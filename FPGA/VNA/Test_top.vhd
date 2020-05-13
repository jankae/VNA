--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   12:35:51 05/08/2020
-- Design Name:   
-- Module Name:   /home/jan/Projekte/VNA/FPGA/VNA/Test_top.vhd
-- Project Name:  VNA
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: top
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY Test_top IS
END Test_top;
 
ARCHITECTURE behavior OF Test_top IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT top
    PORT(
         CLK : IN  std_logic;
         RESET : IN  std_logic;
         MCU_MOSI : IN  std_logic;
         MCU_NSS : IN  std_logic;
         MCU_INTR : OUT  std_logic;
         MCU_SCK : IN  std_logic;
         MCU_MISO : OUT  std_logic;
         MCU_AUX1 : IN  std_logic;
         MCU_AUX2 : IN  std_logic;
         MCU_AUX3 : IN  std_logic;
         SWITCHING_SYNC : OUT  std_logic;
         PORT2_CONVSTART : OUT  std_logic;
         PORT2_SDO : IN  std_logic;
         PORT2_SCLK : OUT  std_logic;
         PORT2_MIX_EN : OUT  std_logic;
         PORT1_CONVSTART : OUT  std_logic;
         PORT1_SDO : IN  std_logic;
         PORT1_SCLK : OUT  std_logic;
         PORT1_MIX_EN : OUT  std_logic;
         LO1_MUX : IN  std_logic;
         LO1_RF_EN : OUT  std_logic;
         LO1_LD : IN  std_logic;
         LO1_CLK : OUT  std_logic;
         LO1_MOSI : OUT  std_logic;
         LO1_LE : OUT  std_logic;
         LO1_CE : OUT  std_logic;
         LEDS : OUT  std_logic_vector(7 downto 0);
         REF_MIX_EN : OUT  std_logic;
         ATTENUATION : OUT  std_logic_vector(6 downto 0);
         AMP_PWDN : OUT  std_logic;
         PORT_SELECT : OUT  std_logic;
         BAND_SELECT : OUT  std_logic;
         FILT_OUT_C1 : OUT  std_logic;
         FILT_OUT_C2 : OUT  std_logic;
         FILT_IN_C1 : OUT  std_logic;
         FILT_IN_C2 : OUT  std_logic;
         SOURCE_RF_EN : OUT  std_logic;
         SOURCE_LD : IN  std_logic;
         SOURCE_MUX : IN  std_logic;
         SOURCE_CLK : OUT  std_logic;
         SOURCE_MOSI : OUT  std_logic;
         SOURCE_LE : OUT  std_logic;
         SOURCE_CE : OUT  std_logic;
         SDA : INOUT  std_logic;
         SCL : INOUT  std_logic;
         REF_CONVSTART : OUT  std_logic;
         REF_SDO : IN  std_logic;
         REF_SCLK : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal CLK : std_logic := '0';
   signal RESET : std_logic := '0';
   signal MCU_MOSI : std_logic := '0';
   signal MCU_NSS : std_logic := '0';
   signal MCU_SCK : std_logic := '0';
   signal MCU_AUX1 : std_logic := '0';
   signal MCU_AUX2 : std_logic := '0';
   signal MCU_AUX3 : std_logic := '0';
   signal PORT2_SDO : std_logic := '0';
   signal PORT1_SDO : std_logic := '0';
   signal LO1_MUX : std_logic := '0';
   signal LO1_LD : std_logic := '0';
   signal SOURCE_LD : std_logic := '0';
   signal SOURCE_MUX : std_logic := '0';
   signal REF_SDO : std_logic := '0';

	--BiDirs
   signal SDA : std_logic;
   signal SCL : std_logic;

 	--Outputs
   signal MCU_INTR : std_logic;
   signal MCU_MISO : std_logic;
   signal SWITCHING_SYNC : std_logic;
   signal PORT2_CONVSTART : std_logic;
   signal PORT2_SCLK : std_logic;
   signal PORT2_MIX_EN : std_logic;
   signal PORT1_CONVSTART : std_logic;
   signal PORT1_SCLK : std_logic;
   signal PORT1_MIX_EN : std_logic;
   signal LO1_RF_EN : std_logic;
   signal LO1_CLK : std_logic;
   signal LO1_MOSI : std_logic;
   signal LO1_LE : std_logic;
   signal LO1_CE : std_logic;
   signal LEDS : std_logic_vector(7 downto 0);
   signal REF_MIX_EN : std_logic;
   signal ATTENUATION : std_logic_vector(6 downto 0);
   signal AMP_PWDN : std_logic;
   signal PORT_SELECT : std_logic;
   signal BAND_SELECT : std_logic;
   signal FILT_OUT_C1 : std_logic;
   signal FILT_OUT_C2 : std_logic;
   signal FILT_IN_C1 : std_logic;
   signal FILT_IN_C2 : std_logic;
   signal SOURCE_RF_EN : std_logic;
   signal SOURCE_CLK : std_logic;
   signal SOURCE_MOSI : std_logic;
   signal SOURCE_LE : std_logic;
   signal SOURCE_CE : std_logic;
   signal REF_CONVSTART : std_logic;
   signal REF_SCLK : std_logic;

   -- Clock period definitions
   constant CLK_period : time := 20 ns;
	constant SPI_CLK_period : time := 100 ns;
	
	signal data_signal : std_logic_vector(15 downto 0); 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: top PORT MAP (
          CLK => CLK,
          RESET => RESET,
          MCU_MOSI => MCU_MOSI,
          MCU_NSS => MCU_NSS,
          MCU_INTR => MCU_INTR,
          MCU_SCK => MCU_SCK,
          MCU_MISO => MCU_MISO,
          MCU_AUX1 => MCU_AUX1,
          MCU_AUX2 => MCU_AUX2,
          MCU_AUX3 => MCU_AUX3,
          SWITCHING_SYNC => SWITCHING_SYNC,
          PORT2_CONVSTART => PORT2_CONVSTART,
          PORT2_SDO => PORT2_SDO,
          PORT2_SCLK => PORT2_SCLK,
          PORT2_MIX_EN => PORT2_MIX_EN,
          PORT1_CONVSTART => PORT1_CONVSTART,
          PORT1_SDO => PORT1_SDO,
          PORT1_SCLK => PORT1_SCLK,
          PORT1_MIX_EN => PORT1_MIX_EN,
          LO1_MUX => LO1_MUX,
          LO1_RF_EN => LO1_RF_EN,
          LO1_LD => LO1_LD,
          LO1_CLK => LO1_CLK,
          LO1_MOSI => LO1_MOSI,
          LO1_LE => LO1_LE,
          LO1_CE => LO1_CE,
          LEDS => LEDS,
          REF_MIX_EN => REF_MIX_EN,
          ATTENUATION => ATTENUATION,
          AMP_PWDN => AMP_PWDN,
          PORT_SELECT => PORT_SELECT,
          BAND_SELECT => BAND_SELECT,
          FILT_OUT_C1 => FILT_OUT_C1,
          FILT_OUT_C2 => FILT_OUT_C2,
          FILT_IN_C1 => FILT_IN_C1,
          FILT_IN_C2 => FILT_IN_C2,
          SOURCE_RF_EN => SOURCE_RF_EN,
          SOURCE_LD => SOURCE_LD,
          SOURCE_MUX => SOURCE_MUX,
          SOURCE_CLK => SOURCE_CLK,
          SOURCE_MOSI => SOURCE_MOSI,
          SOURCE_LE => SOURCE_LE,
          SOURCE_CE => SOURCE_CE,
          SDA => SDA,
          SCL => SCL,
          REF_CONVSTART => REF_CONVSTART,
          REF_SDO => REF_SDO,
          REF_SCLK => REF_SCLK
        );

   -- Clock process definitions
   CLK_process :process
   begin
		CLK <= '0';
		wait for CLK_period/2;
		CLK <= '1';
		wait for CLK_period/2;
   end process;


   -- Stimulus process
   stim_proc: process
			procedure SPI(data : std_logic_vector(15 downto 0)) is
		begin
			MCU_MOSI <= data(15);
			data_signal <= data(14 downto 0) & "0";
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
			MCU_MOSI <= data_signal(15);
			data_signal <= data_signal(14 downto 0) & '0';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '1';
			wait for SPI_CLK_period/2;
			MCU_SCK <= '0';
		end procedure SPI;
   begin		
      -- hold reset state for 100 ns.
		RESET <= '1';
		wait for 1 us;
		MCU_NSS <= '1';
		wait for 1 us;
		RESET <= '0';
		wait for 10 us;
		
		MCU_NSS <= '0';
		SPI("1100000000000000");
		SPI("0000000000000000");
		MCU_NSS <= '1';
		wait;

      -- insert stimulus here 
		-- Test direct connection to source/LO
		MCU_NSS <= '0';
		MCU_AUX1 <= '1';
		wait for CLK_period;
		MCU_SCK <= '1';
		wait for CLK_period;
		MCU_SCK <= '0';
		MCU_MOSI <= '1';
		SOURCE_MUX <= '1';
		wait for CLK_period;
		SOURCE_MUX <= '0';
		MCU_MOSI <= '0';
		wait for CLK_period;
		MCU_AUX1 <= '0';
		MCU_NSS <= '1';
		wait for CLK_period;
		

		-- write number of points
		MCU_NSS <= '0';
		SPI("1000000000000001");
		SPI("0000000000000011");
		MCU_NSS <= '1';
		wait for CLK_period;
		-- Enable mixers/source/LO and write samples/point
		MCU_NSS <= '0';
		SPI("1000000000000011");
		SPI("1111110000000000");
		MCU_NSS <= '1';
		wait for CLK_period;
		MCU_NSS <= '0';
		SPI("1000000000000010");
		SPI("0000000000000011");
		MCU_NSS <= '1';
		wait for CLK_period;
		-- Write default registers
		MCU_NSS <= '0';
		SPI("1000000000001000");
		SPI("0000000000000000");
		MCU_NSS <= '1';
		wait for CLK_period;
		MCU_NSS <= '0';
		SPI("1000000000001001");
		SPI("0000000000000000");
		MCU_NSS <= '1';
		wait for CLK_period;
		MCU_NSS <= '0';
		SPI("1000000000001010");
		SPI("0000000000000000");
		MCU_NSS <= '1';
		wait for CLK_period;
		MCU_NSS <= '0';
		SPI("1000000000001011");
		SPI("0000000000000000");
		MCU_NSS <= '1';
		wait for CLK_period;
		MCU_NSS <= '0';
		SPI("1000000000001100");
		SPI("0000000000000000");
		MCU_NSS <= '1';
		wait for CLK_period;
		MCU_NSS <= '0';
		SPI("1000000000001101");
		SPI("0000000000000000");
		MCU_NSS <= '1';
		wait for CLK_period;
				MCU_NSS <= '0';
		SPI("1000000000001110");
		SPI("0000000000000000");
		MCU_NSS <= '1';
		wait for CLK_period;
		MCU_NSS <= '0';
		SPI("1000000000001111");
		SPI("0000000000000000");
		MCU_NSS <= '1';
		wait for CLK_period;
		-- Write sweep config
		MCU_NSS <= '0';
		SPI("0000000000000000");
		SPI("1111111100000000");
		SPI("1111000011110000");
		SPI("1100110011001100");
		SPI("1010101010101010");
		SPI("1101101101101101");
		SPI("1110111011101110");
		SPI("1111101111101111");
		MCU_NSS <= '1';
		wait for CLK_period;
		MCU_NSS <= '0';
		SPI("0000000000000001");
		SPI("1111111100000000");
		SPI("1111000011110000");
		SPI("1100110011001100");
		SPI("1010101010101010");
		SPI("1101101101101101");
		SPI("1110111011101110");
		SPI("1111101111101111");
		MCU_NSS <= '1';
		wait for CLK_period;
		MCU_NSS <= '0';
		SPI("0000000000000010");
		SPI("1111111100000000");
		SPI("1111000011110000");
		SPI("1100110011001100");
		SPI("1010101010101010");
		SPI("1101101101101101");
		SPI("1110111011101110");
		SPI("1111101111101111");
		MCU_NSS <= '1';
		wait for CLK_period;
		MCU_NSS <= '0';
		SPI("0000000000000011");
		SPI("1111111100000000");
		SPI("1111000011110000");
		SPI("1100110011001100");
		SPI("1010101010101010");
		SPI("1101101101101101");
		SPI("1110111011101110");
		SPI("1111101111101111");
		MCU_NSS <= '1';
		wait for CLK_period;
		
		-- set settling time
		MCU_NSS <= '0';
		SPI("1000000000000100");
		SPI("0010000000000000");
		MCU_NSS <= '1';
		wait for CLK_period;
		-- Start the sweep
		MCU_AUX3 <= '0';
		-- Indicate locked PLLs
		SOURCE_LD <= '1';
		LO1_LD <= '1';
		
		
      wait;
   end process;

END;
