#include <tamtypes.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libpad.h>
#include <gsKit.h>
#include <dmaKit.h>

static char padBuf[256] __attribute__((aligned(64)));

int main(int argc, char *argv[])
{
    // 1. Inicializa o sistema base e os comandos
    SifInitRpc(0);
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);

    padInit(0);
    padPortOpen(0, 0, padBuf);

    // 2. Inicializa o motor gráfico
    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    GSGLOBAL *gsGlobal = gsKit_init_global();
    gsGlobal->Mode = gsKit_check_rom();
    if (gsGlobal->Mode == GS_MODE_PAL)
    {
        gsGlobal->Height = 512;
    }
    else
    {
        gsGlobal->Height = 448;
    }

    gsKit_init_screen(gsGlobal);
    gsKit_mode_switch(gsGlobal, GS_ONESHOT);

    // 3. Paleta de Cores da God Stix Store
    u64 CorFundo = GS_SETREG_RGBAQ(20, 30, 50, 0x00, 0x00);    // Azul Escuro (Fundo)
    u64 CorHeader = GS_SETREG_RGBAQ(200, 30, 30, 0x00, 0x00);  // Vermelho (Barra do topo)
    u64 CorMenu = GS_SETREG_RGBAQ(40, 60, 90, 0x00, 0x00);     // Azul Claro (Caixa central)
    u64 CorCursor = GS_SETREG_RGBAQ(220, 180, 40, 0x00, 0x00); // Amarelo/Dourado (O teu seletor)

    struct padButtonStatus buttons;
    int new_pad;
    int cursor = 0;
    int total_jogos = 5;

    // 4. Loop principal
    while (1)
    {
        // Limpa o ecrã
        gsKit_clear(gsGlobal, CorFundo);

        // Desenha a Interface Base
        gsKit_prim_sprite(gsGlobal, 0, 0, 640, 70, 0, CorHeader);   // Cabeçalho
        gsKit_prim_sprite(gsGlobal, 80, 100, 560, 350, 0, CorMenu); // Menu dos jogos

        // Desenha o Cursor Interativo!
        // A matemática aqui faz o retângulo amarelo descer 45 pixeis de cada vez
        int y_inicial = 115 + (cursor * 45);
        gsKit_prim_sprite(gsGlobal, 90, y_inicial, 550, y_inicial + 35, 0, CorCursor);

        // Envia o frame para a TV
        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);

        // 5. Lê o Comando
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

        // Pausa super rápida para o cursor não voar pelo ecrã
        for (int delay = 0; delay < 3000000; delay++)
            asm("nop");
    }

    return 0;
}