/******************************************************************************
 * Copyright (c) 2017, NXP Semiconductors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/* These are our own includes */
#include <lib/include/agu-tables.h>
#include <lib/include/gtable.h>
#include <lib/include/clock.h>
#include <lib/include/spi.h>
#include <common.h>

int sja1105_cgu_rgmii_tx_clk_config(
		struct sja1105_spi_setup *spi_setup,
		int    port,
		int    speed_mbps)
{
#define MSG_SIZE SIZE_SPI_MSG_HEADER + 4
	struct  sja1105_cgu_mii_control txc;
	struct  sja1105_spi_message msg;
	uint8_t tx_buf[MSG_SIZE];
	uint8_t rx_buf[MSG_SIZE];
	/* UM10944.pdf, Table 78, CGU Register overview */
	int     txc_offsets[] = {0x16, 0x1D, 0x24, 0x2B, 0x32};
	int     clksrc;

	if (speed_mbps == 1000) {
		clksrc = CLKSRC_PLL0;
	} else {
		int clk_sources[] = {CLKSRC_IDIV0, CLKSRC_IDIV1, CLKSRC_IDIV2,
		                     CLKSRC_IDIV3, CLKSRC_IDIV4};
		clksrc = clk_sources[port];
	}

	memset(tx_buf, 0, MSG_SIZE);
	memset(rx_buf, 0, MSG_SIZE);

	/* Header */
	msg.access     = SPI_WRITE;
	msg.read_count = 0;
	msg.address    = CGU_ADDR + txc_offsets[port];
	sja1105_spi_message_set(tx_buf, &msg);

	/* Payload */
	txc.clksrc    = clksrc; /* RGMII: 125MHz for 1000, 25MHz for 100, 2.5MHz for 10 */
	txc.autoblock = 1;      /* Autoblock clk while changing clksrc */
	txc.pd        = 0;      /* Power Down off => enabled */
	sja1105_cgu_mii_control_set(tx_buf + SIZE_SPI_MSG_HEADER, &txc);

	return sja1105_spi_transfer(spi_setup, tx_buf, rx_buf, MSG_SIZE);
}

/* AGU */
int sja1105_rgmii_cfg_pad_tx_config(struct sja1105_spi_setup *spi_setup, int port)
{
#define MSG_SIZE SIZE_SPI_MSG_HEADER + 4
	struct  sja1105_spi_message msg;
	struct  sja1105_cfg_pad_mii_tx pad_mii_tx;
	uint8_t tx_buf[MSG_SIZE];
	uint8_t rx_buf[MSG_SIZE];
	/* UM10944.pdf, Table 86, AGU Register overview */
	int     pad_mii_tx_offsets[] = {0x00, 0x02, 0x04, 0x06, 0x08};

	memset(tx_buf, 0, MSG_SIZE);
	memset(rx_buf, 0, MSG_SIZE);

	/* Header */
	msg.access     = SPI_WRITE;
	msg.read_count = 0;
	msg.address    = AGU_ADDR + pad_mii_tx_offsets[port];
	sja1105_spi_message_set(tx_buf, &msg);

	/* Payload */
	pad_mii_tx.d32_os    = 3; /* TXD[3:2] output stage: high noise/high speed */
	pad_mii_tx.d32_ipud  = 2; /* TXD[3:2] input stage: plain input (default) */
	pad_mii_tx.d10_os    = 3; /* TXD[1:0] output stage: high noise/high speed */
	pad_mii_tx.d10_ipud  = 2; /* TXD[1:0] input stage: plain input (default) */
	pad_mii_tx.ctrl_os   = 3; /* TX_CTL / TX_ER output stage */
	pad_mii_tx.ctrl_ipud = 2; /* TX_CTL / TX_ER input stage (default) */
	pad_mii_tx.clk_os    = 3; /* TX_CLK output stage */
	pad_mii_tx.clk_ih    = 0; /* TX_CLK input hysteresis (default) */
	pad_mii_tx.clk_ipud  = 2; /* TX_CLK input stage (default) */
	sja1105_cfg_pad_mii_tx_set(tx_buf + SIZE_SPI_MSG_HEADER, &pad_mii_tx);

	return sja1105_spi_transfer(spi_setup, tx_buf, rx_buf, MSG_SIZE);
}

int rgmii_clocking_setup(struct sja1105_spi_setup *spi_setup,
                         int port, int speed_mbps)
{
	int rc = 0;

	logv("configuring rgmii clocking for port %d, speed %dMbps",
	     port, speed_mbps);
	if (speed_mbps == 1000) {
		/* 1000Mbps, IDIV disabled, divide by 1 */
		rc = sja1105_cgu_idiv_config(spi_setup, port, 0, 1);
	} else if (speed_mbps == 100) {
		/* 100Mbps, IDIV enabled, divide by 1 */
		rc = sja1105_cgu_idiv_config(spi_setup, port, 1, 1);
	} else if (speed_mbps == 10) {
		/* 10Mbps, IDIV enabled, divide by 10 */
		rc = sja1105_cgu_idiv_config(spi_setup, port, 1, 10);
	}
	if (rc < 0) {
		loge("configuring idiv failed");
		goto out;
	}
	rc = sja1105_cgu_rgmii_tx_clk_config(spi_setup, port, speed_mbps);
	if (rc < 0) {
		loge("configuring rgmii tx clock failed");
		goto out;
	}
	rc = sja1105_rgmii_cfg_pad_tx_config(spi_setup, port);
	if (rc < 0) {
		loge("configuring tx pad registers failed");
		goto out;
	}
out:
	return rc;
}

