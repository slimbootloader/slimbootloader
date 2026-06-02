/*
 * Copyright (c) 2023 Intel Corporation
 */
#ifdef _SLIMBOOTLOADER
#include "ast_drv.h"
#else
#include <linux/firmware.h>
#include <drm/drmP.h>
#include "ast_drv.h"
#endif // _SLIMBOOTLOADER

bool ast_dp_read_edid(struct drm_device *dev, u8 *ediddata)
{
    struct ast_private *ast = dev->dev_private;

    u32 i;

#ifdef DPControlPower
    u8 bDPState_Change = false;

    // Check DP power off or not.
    if(ast->ASTDP_State & 0x10)
    {
        // DP power on
        ast_dp_PowerOnOff(ast->dev, 1);
        bDPState_Change = true;
    }
#endif

    if (ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xDC, 0xFF) == 0)
    {
#ifdef DPControlPower
        // Set back power off
        if(bDPState_Change)
            ast_dp_PowerOnOff(ast->dev, 0);
#endif
        return false;
    }

    ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xE5, 0x00, 0x00);      // reading EDID is NOT done.

    for (i=0; i<64; i++)
    {
        ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xE4, 0x00, (u8) i);

        while ((ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xD7, 0xFF) != 0x01)
                || (ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xD6, 0xFF) != i)){
            msleep(1);
        }

        *(ediddata) = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xD8, 0xFF);
        *(ediddata + 1) = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xD9, 0xFF);
        *(ediddata + 2) = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xDA, 0xFF);
        *(ediddata + 3) = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xDB, 0xFF);

        ediddata += 4;
    }

    ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xE5, 0x00, 0x01);      // reading EDID is DONE.

#ifdef DPControlPower
    // Set back power off
    if(bDPState_Change)
        ast_dp_PowerOnOff(ast->dev, 0);
#endif

    return true;

} /* ReadEDID_AspeedDP */

/*
 * Launch Aspeed DP
 */
bool ast_dp_launch(struct drm_device *dev, u8 bPower)
{
    u32 i = 0,j = 0,WaitCount = 5;
    u8 bDPTX = 0;
    u8 bDPExecute = 1;

    struct ast_private *ast = dev->dev_private;

    // S3 come back, need more time to wait BMC ready.
    if(bPower)
        WaitCount = 300;

    // Fill
    ast->tx_chip_type = AST_TX_NONE;

    // Wait total count by different condition.
    // This is a temp solution for DP check
    for(j=0;j<WaitCount;j++)
    {
        // Wait BMC ready 100ms
        for(i=0;i<100;i++)
        {
            // 1ms
            msleep(1);
        }

        bDPTX = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xD1, 0x0E);

        if(bDPTX)
            break;
    }

    // 0xE : ASTDP with DPMCU FW handling
    if(bDPTX == 0x0E)
    {
        // Wait one second then timeout.
        i = 0;

        while (ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xD1, 0x20) != 0x20)
        {
            i++;
            // wait 1 ms
            msleep(1);

            if(i >= 1000)
            {
                // DP would not be ready.
                bDPExecute = 0;
                break;
            }
        };

        if(bDPExecute)
            ast->tx_chip_type = AST_TX_ASTDP;

    }

    return true;
} /* Launch Aspeed DP */

#ifdef DPControlPower

void ast_dp_PowerOnOff(struct drm_device *dev, u8 Mode)
{
    struct ast_private *ast = dev->dev_private;
    // Read and Turn off DP PHY sleep
    u8 bE3 = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xE3, 0x0F);

    // Turn on DP PHY sleep
    if(!Mode)
        bE3 |= 0x10;

    ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xE3, 0x00, bE3); // DP Power on / off

    // Save ASTDP power state
    ast->ASTDP_State = bE3;

} /* ASTDP_SetOnOff */

#endif

void ast_dp_SetOnOff(struct drm_device *dev, u8 Mode)
{
    struct ast_private *ast = dev->dev_private;

    ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xE3, 0x00, Mode); // video on / off

    // Save ASTDP power state
    ast->ASTDP_State = Mode;

    // If DP plug in and link successful then check video on / off status
    if(ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xDC, 0x01) &&
            ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xDF, 0x01))
    {
        Mode <<= 4;
        while(ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xDF, 0x10) != Mode){
            // wait 1 ms
            msleep(1);
        }
    }
} /* ASTDP_SetOnOff */

void ast_dp_SetOutput(struct drm_crtc *crtc, struct ast_vbios_mode_info *vbios_mode)
{
    struct ast_private *ast = crtc->dev->dev_private;

    u32 ulRefreshRateIndex;
    u8 ModeIdx;

    ulRefreshRateIndex = vbios_mode->enh_table->refresh_rate_index -1;

    DRM_INFO("ulRefreshRateIndex=%x\n", ulRefreshRateIndex);
    DRM_INFO("crtc_hdisplay=%d, crtc_vdisplay=%d\n", crtc->mode.crtc_hdisplay, crtc->mode.crtc_vdisplay);

    switch (crtc->mode.crtc_hdisplay)
    {
        case 320:
            ModeIdx = 0x11;
            break;
        case 400:
            ModeIdx = 0x12;
            break;
        case 512:
            ModeIdx = 0x13;
            break;
        case 640:
            ModeIdx = (0x00 + (u8) ulRefreshRateIndex);
            break;
        case 800:
            ModeIdx = (0x04 + (u8) ulRefreshRateIndex);
            break;
        case 1024:
            ModeIdx = (0x09 + (u8) ulRefreshRateIndex);
            break;
        case 1152:
            ModeIdx = 0x1F;
            break;
        case 1280:
            if (crtc->mode.crtc_vdisplay == 800)
                ModeIdx = (0x17 - (u8) ulRefreshRateIndex); // For RB/Non-RB
            else       // 1024
                ModeIdx = (0x0D + (u8) ulRefreshRateIndex);
            break;
        case 1360:
        case 1366:
            ModeIdx = 0x1E;
            break;
        case 1440:
            ModeIdx = (0x19 - (u8) ulRefreshRateIndex); // For RB/Non-RB
            break;
        case 1600:
            if (crtc->mode.crtc_vdisplay == 900)
                ModeIdx = (0x1D - (u8) ulRefreshRateIndex); // For RB/Non-RB
            else       //1200
                ModeIdx = 0x10;
            break;
        case 1680:
            ModeIdx = (0x1B - (u8) ulRefreshRateIndex); // For RB/Non-RB
            break;
        case 1920:
            if (crtc->mode.crtc_vdisplay == 1080)
                ModeIdx = 0x15;
            else       //1200
                ModeIdx = 0x14;
            break;
        default:
            return;
    }

    ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xE5, 0x00, 0x01);        // reading EDID is DONE.
    ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xE0, 0x00, 0x20);        // MISC0,0x00: 18-bpp,0x20: 24-bp
    ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xE1, 0x00, 0x00);        // MISC1 (default: 0x00)
    ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xE2, 0x00, ModeIdx);     // indicate mode number

    return;

} //ASTDP_SetOutput
