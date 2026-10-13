#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <libpad.h>

static char padBuf[256] __attribute__((aligned(64)));

int main(int argc, char *argv[])
{
    // Inicialização do sistema e comando
    SifInitRpc(0);
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);

    init_scr();
    padInit(0);
    padPortOpen(0, 0, padBuf);

    // Lista de jogos disponíveis na nossa loja
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

    // Loop principal da loja
    while (1)
    {
        // 1. Limpa o ecrã para atualizar a posição da seta
        scr_clear();

        // 2. Desenha o cabeçalho
        scr_printf("==================================================\n");
        scr_printf("                 GOD STIX STORE                   \n");
        scr_printf("==================================================\n\n");
        scr_printf(" Selecione um jogo para baixar (Setas Cima/Baixo):\n\n");

        // 3. Desenha a lista de jogos
        for (int i = 0; i < total_jogos; i++)
        {
            if (i == cursor)
            {
                // Se o cursor estiver neste jogo, desenha uma seta a apontar
                scr_printf("  -> [ %s ] <-\n", jogos[i]);
            }
            else
            {
                // Jogos normais
                scr_printf("     %s\n", jogos[i]);
            }
        }

        scr_printf("\n Pressione [X] para confirmar\n");

        // Pausa muito rápida para o ecrã não piscar e os botões não ficarem loucos
        for (int delay = 0; delay < 5000000; delay++)
            asm("nop");

        // 4. Lê os botões do comando
        int state = padGetState(0, 0);
        if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1)
        {
            padRead(0, 0, &buttons);
            new_pad = 0xffff ^ buttons.btns;

            if (new_pad & PAD_UP)
            { // Seta para cima
                cursor--;
                if (cursor < 0)
                    cursor = total_jogos - 1; // Se passar do topo, vai para o fundo
            }
            if (new_pad & PAD_DOWN)
            { // Seta para baixo
                cursor++;
                if (cursor >= total_jogos)
                    cursor = 0; // Se passar do fundo, volta ao topo
            }
            if (new_pad & PAD_CROSS)
            { // Botão X
                scr_clear();
                scr_printf("\n\n  [+] A preparar o download de:\n      %s\n", jogos[cursor]);
                break; // Sai do menu para a fase de "download"
            }
        }
    }

    // Loop final depois de escolher o jogo
    while (1)
    {
    }

    return 0;
}