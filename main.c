#include <tamtypes.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libpad.h>
#include <gsKit.h>
#include <dmaKit.h>
#include <gsFontM.h> // A biblioteca mágica que rouba a fonte da BIOS da PS2

static char padBuf[256] __attribute__((aligned(64)));

int main(int argc, char *argv[])
{
    // 1. Inicializa Sistema e Comando
    SifInitRpc(0);
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);

    padInit(0);
    padPortOpen(0, 0, padBuf);

    // 2. Inicializa Motor Gráfico
    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    GSGLOBAL *gsGlobal = gsKit_init_global();
    gsGlobal->Mode = gsKit_check_rom();
    gsGlobal->Height = (gsGlobal->Mode == GS_MODE_PAL) ? 512 : 448;

    gsKit_init_screen(gsGlobal);
    gsKit_mode_switch(gsGlobal, GS_ONESHOT);

    // 3. Inicializa a Fonte da BIOS da PS2
    GSFONTM *gsFontM = gsKit_fontm_init(gsGlobal);
    gsKit_fontm_upload(gsGlobal, gsFontM);
    gsFontM->Spacing = 0.9f;

    // 4. Paleta de Cores Definitiva (Modo Escuro)
    u64 CorFundo = GS_SETREG_RGBAQ(15, 15, 15, 0x00, 0x00);          // Preto/Cinza muito escuro
    u64 CorHeader = GS_SETREG_RGBAQ(0, 112, 204, 0x00, 0x00);        // Azul PlayStation Clássico
    u64 CorCursor = GS_SETREG_RGBAQ(255, 255, 255, 0x00, 0x00);      // Branco puro
    u64 CorTextoBranco = GS_SETREG_RGBAQ(255, 255, 255, 0x00, 0x00); // Texto normal
    u64 CorTextoPreto = GS_SETREG_RGBAQ(0, 0, 0, 0x00, 0x00);        // Texto quando selecionado
    u64 CorRodape = GS_SETREG_RGBAQ(30, 30, 30, 0x00, 0x00);         // Cinza escuro para o rodapé

    char *jogos[] = {
        "God of War II",
        "Grand Theft Auto: San Andreas",
        "Black",
        "Need for Speed: Underground 2",
        "Resident Evil 4",
        "Bully",
        "Shadow of the Colossus"};
    int total_jogos = 7;
    int cursor = 0;

    struct padButtonStatus buttons;
    int new_pad;

    // 5. Loop do Visual Final
    while (1)
    {
        gsKit_clear(gsGlobal, CorFundo);

        // Desenha o Cabeçalho
        gsKit_prim_sprite(gsGlobal, 0, 0, 640, 60, 0, CorHeader);
        gsKit_fontm_print_scaled(gsGlobal, gsFontM, 20, 15, 1, 1.2f, CorTextoBranco, "GOD STIX STORE");

        // Desenha o Rodapé com os controlos
        gsKit_prim_sprite(gsGlobal, 0, gsGlobal->Height - 40, 640, gsGlobal->Height, 0, CorRodape);
        gsKit_fontm_print_scaled(gsGlobal, gsFontM, 20, gsGlobal->Height - 30, 1, 0.8f, CorTextoBranco, "Pressione [X] para Baixar   |   [Setas] para Navegar");

        // Desenha a Lista de Jogos Dinâmica
        int start_y = 100;
        for (int i = 0; i < total_jogos; i++)
        {
            int y_pos = start_y + (i * 40);

            if (i == cursor)
            {
                // Se estiver selecionado: Fundo Branco, Letra Preta
                gsKit_prim_sprite(gsGlobal, 30, y_pos - 5, 610, y_pos + 30, 0, CorCursor);
                gsKit_fontm_print_scaled(gsGlobal, gsFontM, 45, y_pos, 1, 1.0f, CorTextoPreto, jogos[i]);
            }
            else
            {
                // Se não estiver selecionado: Letra Branca, sem fundo
                gsKit_fontm_print_scaled(gsGlobal, gsFontM, 45, y_pos, 1, 1.0f, CorTextoBranco, jogos[i]);
            }
        }

        // Renderiza tudo na TV
        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);

        // Controlos de Navegação
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

        // Pausa para estabilizar o movimento das setas
        for (int delay = 0; delay < 4000000; delay++)
            asm("nop");
    }

    return 0;
}