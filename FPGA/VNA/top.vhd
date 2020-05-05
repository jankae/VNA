----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    15:47:31 05/05/2020 
-- Design Name: 
-- Module Name:    top - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity top is
    Port ( CLK : in  STD_LOGIC;
           RESET : in  STD_LOGIC;
           MCU_MOSI : in  STD_LOGIC;
           MCU_NSS : in  STD_LOGIC;
           MCU_INTR : out  STD_LOGIC;
           MCU_SCK : in  STD_LOGIC;
           MCU_MISO : out  STD_LOGIC;
           MCU_AUX1 : in  STD_LOGIC;
           MCU_AUX2 : in  STD_LOGIC;
           MCU_AUX3 : in  STD_LOGIC;
           SWITCHING_SYNC : out  STD_LOGIC;
           PORT2_CONVSTART : out  STD_LOGIC;
           PORT2_SDO : in  STD_LOGIC;
           PORT2_SCLK : out  STD_LOGIC;
           PORT2_MIX_EN : out  STD_LOGIC;
           PORT1_CONVSTART : out  STD_LOGIC;
           PORT1_SDO : in  STD_LOGIC;
           PORT1_SCLK : out  STD_LOGIC;
           PORT1_MIX_EN : out  STD_LOGIC;
           LO1_MUX : in  STD_LOGIC;
           LO1_RF_EN : out  STD_LOGIC;
           LO1_LD : in  STD_LOGIC;
           LO1_CLK : out  STD_LOGIC;
           LO1_MOSI : out  STD_LOGIC;
           LO1_LE : out  STD_LOGIC;
           LO1_CE : out  STD_LOGIC;
           LEDS : out  STD_LOGIC_VECTOR (7 downto 0);
           REF_MIX_EN : out  STD_LOGIC;
           ATTENUATION : out  STD_LOGIC_VECTOR (6 downto 0);
           AMP_PWDN : out  STD_LOGIC;
           PORT_SELECT : out  STD_LOGIC;
           BAND_SELECT : out  STD_LOGIC;
           FILT_OUT_C1 : out  STD_LOGIC;
           FILT_OUT_C2 : out  STD_LOGIC;
           FILT_IN_C1 : out  STD_LOGIC;
           FILT_IN_C2 : out  STD_LOGIC;
           SOURCE_RF_EN : out  STD_LOGIC;
           SOURCE_LD : in  STD_LOGIC;
           SOURCE_MUX : in  STD_LOGIC;
           SOURCE_CLK : out  STD_LOGIC;
           SOURCE_MOSI : out  STD_LOGIC;
           SOURCE_LE : out  STD_LOGIC;
           SOURCE_CE : out  STD_LOGIC;
           SDA : inout  STD_LOGIC;
           SCL : inout  STD_LOGIC;
           REF_CONVSTART : out  STD_LOGIC;
           REF_SDO : in  STD_LOGIC;
           REF_SCLK : out  STD_LOGIC);
end top;

architecture Behavioral of top is

begin


end Behavioral;

