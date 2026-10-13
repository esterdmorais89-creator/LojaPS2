#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <libpad.h>
#include <loadfile.h>

static char padBuf[256] __attribute__((aligned(64)));

int main(int argc, char *argv[])
{
    // 1. Inicialização do sistema e comando
    SifInitRpc(0);
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);

    init_scr();
    padInit(0);
    padPortOpen(0, 0, padBuf);

    // 2. Lista de jogos disponíveis na nossa loja
    char *jogos[] = {
        "God of War II",
        "Grand Theft Auto: San Andreas",
        "Black",
        "Need for Speed: Underground 2",
        "Resident Evil 4"};
    int total_jogos = 5;
    int cursor = 0; // Vai dizer qual é o jogo selecionado (0 a 4)

    struct padButtonStatus buttons;
    int new_pad;

    // 3. Loop principal da loja (O menu fica aqui dentro a atualizar)
    while (1)
    {
        // Limpa o ecrã para atualizar a posição da seta
        scr_clear();

        // Desenha o cabeçalho
        scr_printf("==================================================\n");
        scr_printf("                 GOD STIX STORE                   \n");
        scr_printf("==================================================\n\n");
        scr_printf(" Selecione um jogo para baixar (Setas Cima/Baixo):\n\n");

        // Desenha a lista de jogos
        for (int i = 0; i < total_jogos; i++)
        {
            if (i == cursor)
            {
                // Se o cursor estiver neste jogo, desenha uma seta a apontar
                scr_printf("  -> [ %s ] <-\n", jogos[i]);
            }
            else
            {
                // Jogos normais sem seleção
                scr_printf("     %s\n", jogos[i]);
            }
        }

        scr_printf("\n Pressione [X] para confirmar\n");

        // Pausa muito rápida para o ecrã não piscar e os botões não ficarem muito sensíveis
        for (int delay = 0; delay < 5000000; delay++)
            asm("nop");

        // Lê os botões do comando
        int state = padGetState(0, 0);
        if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1)
        {
            padRead(0, 0, &buttons);
            new_pad = 0xffff ^ buttons.btns;

            // Navegação nas setas
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

            // Quando carrega no botão X (Confirmação)
            if (new_pad & PAD_CROSS)
            {
                scr_clear();
                scr_printf("\n\n  [+] A iniciar o download de:\n      %s\n\n", jogos[cursor]);

                scr_printf("  Progresso: [");

                // Simula uma barra de progresso a encher com '#'
                for (int p = 0; p <= 20; p++)
                {
                    scr_printf("#");

                    // Pausa para a barra não encher instantaneamente (simula tempo de rede)
                    for (int delay = 0; delay < 15000000; delay++)
                        asm("nop");
                }

                scr_printf("] 100%%\n\n");
                scr_printf("  [+] Download Concluido com Sucesso!\n");
                scr_printf("  Pressione [O] (Bolinha) para voltar ao menu.\n");

                // Fica preso aqui a aguardar que carregues na Bolinha para voltar ao menu
                while (1)
                {
                    padRead(0, 0, &buttons);
                    int bolinha_pad = 0xffff ^ buttons.btns;
                    if (bolinha_pad & PAD_CIRCLE)
                    {
                        break; // Ao quebrar este pequeno loop, ele volta ao início do menu!
                    }
                }
            }
        }
    }

    return 0;
}