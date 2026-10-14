#include <tamtypes.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libpad.h>
#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h> // A nova biblioteca para ler BMPs

static char padBuf[256] __attribute__((aligned(64)));

int main(int argc, char *argv[])
{
    // 1. Inicializa o Sistema
    SifInitRpc(0);
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);

    padInit(0);
    padPortOpen(0, 0, padBuf);

    // 2. Motor Gráfico
    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    GSGLOBAL *gsGlobal = gsKit_init_global();
    gsGlobal->Mode = gsKit_check_rom();
    gsGlobal->Height = (gsGlobal->Mode == GS_MODE_PAL) ? 512 : 448;

    gsKit_init_screen(gsGlobal);
    gsGlobal->PrimAlphaEnable = GS_SETTING_ON; // Ativa transparências para o cursor
    gsKit_mode_switch(gsGlobal, GS_ONESHOT);

    // 3. Carregar as Imagens BMP
    GSTEXTURE tex_abertura;
    GSTEXTURE tex_fundo;

    // Procura as imagens na pasta partilhada do emulador (host)
    gsKit_texture_bmp(gsGlobal, &tex_abertura, "host:imagens/abertura.bmp");
    gsKit_texture_bmp(gsGlobal, &tex_fundo, "host:imagens/fundo.bmp");

    // --- CENA DE ABERTURA ---
    gsKit_clear(gsGlobal, GS_SETREG_RGBAQ(0, 0, 0, 0, 0));
    // Desenha a imagem de abertura a ocupar o ecrã todo
    gsKit_prim_sprite_texture(gsGlobal, &tex_abertura, 0.0f, 0.0f, 0.0f, 0.0f, 640.0f, (float)gsGlobal->Height, (float)tex_abertura.Width, (float)tex_abertura.Height, 0, GS_SETREG_RGBAQ(0x80, 0x80, 0x80, 0x80, 0x00));
    gsKit_queue_exec(gsGlobal);
    gsKit_sync_flip(gsGlobal);

    // Pausa dramática para exibir a intro
    for (int delay = 0; delay < 20000000; delay++)
        asm("nop");

    // --- MENU PRINCIPAL DA LOJA ---
    // Cursor Amarelo Dourado com alguma transparência (para se ver um pouco o fundo através dele)
    u64 CorCursor = GS_SETREG_RGBAQ(255, 215, 0, 100, 0x00);

    struct padButtonStatus buttons;
    int new_pad;
    int cursor = 0;
    int total_jogos = 7;

    while (1)
    {
        gsKit_clear(gsGlobal, GS_SETREG_RGBAQ(0, 0, 0, 0, 0));

        // Cola a imagem de fundo profissional (que já tem o texto escrito)
        gsKit_prim_sprite_texture(gsGlobal, &tex_fundo, 0.0f, 0.0f, 0.0f, 0.0f, 640.0f, (float)gsGlobal->Height, (float)tex_fundo.Width, (float)tex_fundo.Height, 0, GS_SETREG_RGBAQ(0x80, 0x80, 0x80, 0x80, 0x00));

        // Desenha o Cursor Amarelo a deslizar ao lado dos nomes
        int start_y = 75;                    // Altura onde começa o primeiro jogo na imagem
        int y_pos = start_y + (cursor * 45); // Move 45 pixeis para baixo por cada jogo
        // O cursor vai apenas cobrir a área do texto
        gsKit_prim_sprite(gsGlobal, 45.0f, (float)y_pos, 600.0f, (float)(y_pos + 40), 1, CorCursor);

        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);

        // Lógica do Comando
        int state = padGetState(0, 0);
        if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1)
        {
            padRead(0, 0, &buttons);
            new_pad = 0xffff ^ buttons.btns;

            if (new_pad & PAD_UP)
            {
                cursor--;
                if (cursor < 0)
                    cursor = total_jogos - 1;
            }
            if (new_pad & PAD_DOWN)
            {
                cursor++;
                if (cursor >= total_jogos)
                    cursor = 0;
            }
        }

        for (int delay = 0; delay < 3500000; delay++)
            asm("nop");
    }

    return 0;
}