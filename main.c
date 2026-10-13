#include <tamtypes.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libpad.h>
#include <gsKit.h>
#include <dmaKit.h>

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
    gsKit_mode_switch(gsGlobal, GS_ONESHOT);

    // 3. A TUA NOVA PALETA DE CORES (Sony / PS2on Theme)
    u64 CorFundo = GS_SETREG_RGBAQ(170, 10, 10, 0x00, 0x00);       // Vermelho vibrante
    u64 CorRodape = GS_SETREG_RGBAQ(110, 0, 0, 0x00, 0x00);        // Vermelho muito escuro (quase vinho)
    u64 CorDetalhe = GS_SETREG_RGBAQ(255, 255, 255, 0x00, 0x00);   // Branco para destacar as áreas de texto
    u64 CorCursor = GS_SETREG_RGBAQ(255, 215, 0, 0x00, 0x00);      // Amarelo / Dourado Premium para a seleção
    u64 CorPlaceholder = GS_SETREG_RGBAQ(190, 30, 30, 0x00, 0x00); // Espaços onde vão ficar as palavras

    struct padButtonStatus buttons;
    int new_pad;
    int cursor = 0;
    int total_jogos = 7;

    while (1)
    {
        // Pinta o ecrã com o teu fundo vermelho
        gsKit_clear(gsGlobal, CorFundo);

        // Barra decorativa superior e inferior (Estilo Adesivo SONY)
        gsKit_prim_sprite(gsGlobal, 0, 0, 640, 50, 0, CorRodape);
        gsKit_prim_sprite(gsGlobal, 0, gsGlobal->Height - 50, 640, gsGlobal->Height, 0, CorRodape);

        // Linha divisória branca (design limpo)
        gsKit_prim_sprite(gsGlobal, 0, 50, 640, 52, 0, CorDetalhe);
        gsKit_prim_sprite(gsGlobal, 0, gsGlobal->Height - 52, 640, gsGlobal->Height - 50, 0, CorDetalhe);

        // Desenha a Lista de Jogos
        int start_y = 90;
        for (int i = 0; i < total_jogos; i++)
        {
            int y_pos = start_y + (i * 45);

            if (i == cursor)
            {
                // SELECIONADO: O cursor Dourado e maior
                gsKit_prim_sprite(gsGlobal, 20, y_pos, 620, y_pos + 38, 0, CorCursor);
            }
            else
            {
                // NÃO SELECIONADO: Caixas vermelho-claro (onde vamos pôr as letras depois)
                gsKit_prim_sprite(gsGlobal, 30, y_pos + 4, 610, y_pos + 34, 0, CorPlaceholder);
            }
        }

        // Envia para a TV (Emulador)
        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);

        // Lógica do Comando (Setas)
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

        // Estabilizador super-rápido para o cursor não deslizar muito rápido
        for (int delay = 0; delay < 3500000; delay++)
            asm("nop");
    }

    return 0;
}