#include <tamtypes.h>
#include <sifrpc.h>
#include <libpad.h>
#include <gsKit.h>
#include <dmaKit.h>

int main(int argc, char *argv[])
{
    // 1. Inicializa o sistema base
    SifInitRpc(0);

    // 2. Inicializa o motor de transferência de dados (DMA)
    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    // 3. Inicializa a Placa Gráfica da PS2 (GS)
    GSGLOBAL *gsGlobal = gsKit_init_global();

    // Configura a resolução para a TV normal (PAL/NTSC)
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

    // 4. Define as cores reais (Formato: R, G, B, Transparência)
    u64 CorFundo = GS_SETREG_RGBAQ(20, 30, 50, 0x00, 0x00); // Azul Escuro

    // 5. Loop principal de desenho gráfico
    while (1)
    {
        // Limpa o ecrã com a nossa cor de fundo
        gsKit_clear(gsGlobal, CorFundo);

        // Aqui dentro é onde vamos desenhar imagens, botões e listas no futuro!

        // Envia o desenho para a TV/Emulador
        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);
    }

    return 0;
}