#include <tamtypes.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libpad.h>
#include <gsKit.h>
#include <dmaKit.h>
#include <gsFontM.h>

static char padBuf[256] __attribute__((aligned(64)));

int main(int argc, char *argv[])
{
    // Inicialização do sistema
    SifInitRpc(0);
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);

    padInit(0);
    padPortOpen(0, 0, padBuf);

    // Inicialização Gráfica
    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    GSGLOBAL *gsGlobal = gsKit_init_global();
    gsGlobal->Mode = gsKit_check_rom();
    gsGlobal->Height = (gsGlobal->Mode == GS_MODE_PAL) ? 512 : 448;

    gsKit_init_screen(gsGlobal);
    gsKit_mode_switch(gsGlobal, GS_ONESHOT);

    // Inicializa a Fonte da BIOS
    GSFONTM *gsFontM = gsKit_fontm_init(gsGlobal);
    gsKit_fontm_upload(gsGlobal, gsFontM);
    gsFontM->Spacing = 0.9f;

    // --- A TUA NOVA PALETA DE CORES ---
    u64 CorFundo = GS_SETREG_RGBAQ(180, 15, 15, 0x00, 0x00);   // Fundo Vermelho
    u64 CorAdesivo = GS_SETREG_RGBAQ(120, 5, 5, 0x00, 0x00);   // Vermelho Escuro para a marca d'água no fundo
    u64 CorTexto = GS_SETREG_RGBAQ(255, 255, 255, 0x00, 0x00); // Letras Brancas
    u64 CorTextoSel = GS_SETREG_RGBAQ(0, 0, 0, 0x00, 0x00);    // Letras Pretas (quando selecionado)
    u64 CorCursor = GS_SETREG_RGBAQ(255, 200, 0, 0x00, 0x00);  // Cursor Amarelo Estilo Premium

    char *jogos[] = {
        "God of War II",
        "GTA San Andreas",
        "Black",
        "Need for Speed Underground 2",
        "Resident Evil 4",
        "Bully",
        "Shadow of the Colossus"};
    int total_jogos = 7;
    int cursor = 0;

    struct padButtonStatus buttons;
    int new_pad;

    while (1)
    {
        // Limpa o ecrã com o vermelho principal
        gsKit_clear(gsGlobal, CorFundo);

        // --- O ADESIVO / MARCA D'ÁGUA DA SONY NO FUNDO ---
        // Desenhado gigante, rodado no fundo e com cor escura para dar um ar profissional
        gsKit_fontm_print_scaled(gsGlobal, gsFontM, 20, 120, 1, 4.0f, CorAdesivo, "SONY");
        gsKit_fontm_print_scaled(gsGlobal, gsFontM, 20, 240, 1, 2.5f, CorAdesivo, "default ps2on");

        // Linha divisória de design
        gsKit_prim_sprite(gsGlobal, 0, 80, 640, 85, 0, CorAdesivo);

        // --- DESENHA A LISTA DE JOGOS ---
        int start_y = 100;
        for (int i = 0; i < total_jogos; i++)
        {
            int y_pos = start_y + (i * 45);

            if (i == cursor)
            {
                // Fundo amarelo com letras pretas
                gsKit_prim_sprite(gsGlobal, 15, y_pos - 5, 625, y_pos + 35, 0, CorCursor);
                gsKit_fontm_print_scaled(gsGlobal, gsFontM, 30, y_pos, 1, 1.2f, CorTextoSel, jogos[i]);
            }
            else
            {
                // Sem fundo, letras brancas
                gsKit_fontm_print_scaled(gsGlobal, gsFontM, 30, y_pos, 1, 1.2f, CorTexto, jogos[i]);
            }
        }

        // Renderiza tudo
        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);

        // Controlos
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

        // Pausa para estabilidade
        for (int delay = 0; delay < 4000000; delay++)
            asm("nop");
    }

    return 0;
}