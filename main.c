#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>

// Atalhos para os códigos de cor ANSI
#define COR_VERMELHA "\033[31m"
#define COR_VERDE "\033[32m"
#define COR_AMARELA "\033[33m"
#define COR_AZUL "\033[34m"
#define COR_CIANO "\033[36m"
#define COR_BRANCA "\033[37m"
#define COR_RESET "\033[0m" // Volta à cor padrão

int main(int argc, char *argv[])
{
    SifInitRpc(0);
    init_scr();
    scr_clear();

    // Aplica a cor ciano aos separadores e amarelo ao título
    scr_printf(COR_CIANO "==================================================\n");
    scr_printf(COR_AMARELA "                 GOD STIX STORE                   \n");
    scr_printf(COR_CIANO "==================================================\n\n" COR_RESET);

    // Aplica verde para as mensagens de sucesso
    scr_printf(COR_VERDE "  [+] Interface visual carregada com sucesso!\n");
    scr_printf("  [+] Versao: 0.1 Alpha\n\n" COR_RESET);

    // Aplica azul (ou outra cor da tua escolha) para o estado da ligação
    scr_printf(COR_BRANCA "  A aguardar ligacao aos servidores...\n" COR_RESET);

    while (1)
    {
    }

    return 0;
}