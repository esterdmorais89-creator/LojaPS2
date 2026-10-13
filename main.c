#include <tamtypes.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libpad.h>
#include <gsKit.h>
#include <dmaKit.h>

static char padBuf[256] __attribute__((aligned(64)));

int main(int argc, char *argv[])
{
    // 1. Inicializa o Sistema e Comando
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

    // 3. PALETA DE CORES
    u64 CorFundo = GS_SETREG_RGBAQ(170, 10, 10, 0x00, 0x00);       // Vermelho vibrante
    u64 CorRodape = GS_SETREG_RGBAQ(110, 0, 0, 0x00, 0x00);        // Vermelho muito escuro
    u64 CorDetalhe = GS_SETREG_RGBAQ(255, 255, 255, 0x00, 0x00);   // Branco
    u64 CorCursor = GS_SETREG_RGBAQ(255, 215, 0, 0x00, 0x00);      // Amarelo Dourado Premium
    u64 CorPlaceholder = GS_SETREG_RGBAQ(190, 30, 30, 0x00, 0x00); // Caixas vermelhas-claras

    // Cores do Adesivo
    u64 CorPrata = GS_SETREG_RGBAQ(200, 200, 210, 0x00, 0x00); // Prateado do Adesivo
    u64 CorAzulSony = GS_SETREG_RGBAQ(0, 50, 255, 0x00, 0x00); // Azul clássico

    // --- ANIMAÇÃO DE ABERTURA (INTRO) ---
    // Passo 1: Fundo preto com linha dourada a crescer
    for (int step = 0; step < 120; step++)
    {
        gsKit_clear(gsGlobal, GS_SETREG_RGBAQ(0, 0, 0, 0x00, 0x00)); // Preto

        // A linha cresce do centro para as bordas
        int tamanho = step * 2;
        gsKit_prim_sprite(gsGlobal, 320 - tamanho, 250, 320 + tamanho, 255, 0, CorCursor);

        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);

        // Atraso para a animação não ser instantânea
        for (int delay = 0; delay < 1500000; delay++)
            asm("nop");
    }

    // Passo 2: Flash Branco super rápido
    for (int flash = 0; flash < 8; flash++)
    {
        gsKit_clear(gsGlobal, CorDetalhe);
        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);
        for (int delay = 0; delay < 2000000; delay++)
            asm("nop");
    }
    // --- FIM DA ANIMAÇÃO ---

    // Variáveis da Loja
    struct padButtonStatus buttons;
    int new_pad;
    int cursor = 0;
    int total_jogos = 7;

    // LOOP PRINCIPAL DA LOJA
    while (1)
    {
        gsKit_clear(gsGlobal, CorFundo);

        // Barras superior e inferior
        gsKit_prim_sprite(gsGlobal, 0, 0, 640, 50, 0, CorRodape);
        gsKit_prim_sprite(gsGlobal, 0, gsGlobal->Height - 50, 640, gsGlobal->Height, 0, CorRodape);

        // Linhas decorativas brancas
        gsKit_prim_sprite(gsGlobal, 0, 50, 640, 52, 0, CorDetalhe);
        gsKit_prim_sprite(gsGlobal, 0, gsGlobal->Height - 52, 640, gsGlobal->Height - 50, 0, CorDetalhe);

        // --- ADESIVO "DEFAULT PS2ON / SONY" ---
        // Desenhado no rodapé à direita
        int h = gsGlobal->Height;
        gsKit_prim_sprite(gsGlobal, 480, h - 45, 620, h - 5, 0, CorPrata);                           // Base prateada do adesivo
        gsKit_prim_sprite(gsGlobal, 485, h - 40, 510, h - 10, 0, CorAzulSony);                       // Logotipo/chip Azul holográfico
        gsKit_prim_sprite(gsGlobal, 520, h - 35, 610, h - 30, 0, GS_SETREG_RGBAQ(50, 50, 50, 0, 0)); // Linha simulando texto
        gsKit_prim_sprite(gsGlobal, 520, h - 25, 590, h - 20, 0, GS_SETREG_RGBAQ(50, 50, 50, 0, 0)); // Linha simulando texto
        // --------------------------------------

        // Lista de Jogos (Caixas)
        int start_y = 90;
        for (int i = 0; i < total_jogos; i++)
        {
            int y_pos = start_y + (i * 45);

            if (i == cursor)
            {
                // Caixa Selecionada (Dourada)
                gsKit_prim_sprite(gsGlobal, 20, y_pos, 620, y_pos + 38, 0, CorCursor);
            }
            else
            {
                // Caixa Normal (Vermelho mais claro)
                gsKit_prim_sprite(gsGlobal, 30, y_pos + 4, 610, y_pos + 34, 0, CorPlaceholder);
            }
        }

        // Renderiza
        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);

        // Controlos das Setas
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

        // Pausa do comando
        for (int delay = 0; delay < 3500000; delay++)
            asm("nop");
    }

    return 0;
}