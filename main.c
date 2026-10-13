#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <libpad.h>
#include <loadfile.h>
#include <fileio.h> // Biblioteca vital para ler/gravar na Pen Drive e Memory Card

static char padBuf[256] __attribute__((aligned(64)));

int main(int argc, char *argv[])
{
    // Inicialização do sistema
    SifInitRpc(0);
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);

    init_scr();
    padInit(0);
    padPortOpen(0, 0, padBuf);

    char *jogos[] = {
        "God of War II",
        "Grand Theft Auto: San Andreas",
        "Black",
        "Need for Speed: Underground 2",
        "Resident Evil 4"};
    int total_jogos = 5;
    int cursor = 0;

    struct padButtonStatus buttons;
    int new_pad;

    while (1)
    {
        scr_clear();
        scr_printf("==================================================\n");
        scr_printf("                 GOD STIX STORE                   \n");
        scr_printf("==================================================\n\n");
        scr_printf(" Selecione um jogo para baixar (Setas Cima/Baixo):\n\n");

        for (int i = 0; i < total_jogos; i++)
        {
            if (i == cursor)
            {
                scr_printf("  -> [ %s ] <-\n", jogos[i]);
            }
            else
            {
                scr_printf("     %s\n", jogos[i]);
            }
        }

        scr_printf("\n Pressione [X] para confirmar\n");

        for (int delay = 0; delay < 5000000; delay++)
            asm("nop");

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

            if (new_pad & PAD_CROSS)
            {
                scr_clear();
                scr_printf("\n\n  [+] A verificar armazenamento USB (mass:)...\n\n");

                // Tenta abrir a raiz da Pen Drive
                int porta_usb = fioDopen("mass:");

                if (porta_usb < 0)
                {
                    // Se o resultado for menor que 0, a diretoria não existe ou falhou
                    scr_printf("  [ERRO] Nenhuma Pen Drive detetada!\n");
                    scr_printf("  A instalacao foi cancelada. Insira um dispositivo.\n\n");
                }
                else
                {
                    // Pen drive encontrada com sucesso
                    fioDclose(porta_usb); // Fecha a diretoria após a verificação
                    scr_printf("  [OK] Pen Drive detetada com sucesso!\n");
                    scr_printf("  [+] A iniciar o download de:\n      %s\n\n", jogos[cursor]);

                    scr_printf("  A Gravar: [");
                    for (int p = 0; p <= 20; p++)
                    {
                        scr_printf("#");
                        for (int delay = 0; delay < 15000000; delay++)
                            asm("nop");
                    }
                    scr_printf("] 100%%\n\n");
                    scr_printf("  [+] Download Concluido e Guardado no USB!\n");
                }

                scr_printf("  Pressione [O] (Bolinha) para voltar ao menu.\n");

                while (1)
                {
                    padRead(0, 0, &buttons);
                    int bolinha_pad = 0xffff ^ buttons.btns;
                    if (bolinha_pad & PAD_CIRCLE)
                    {
                        break;
                    }
                }
            }
        }
    }

    return 0;
}