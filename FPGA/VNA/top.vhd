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
	component PLL
	port
		(-- Clock in ports
		CLK_IN1           : in     std_logic;
		-- Clock out ports
		CLK_OUT1          : out    std_logic;
		-- Status and control signals
		RESET             : in     std_logic;
		LOCKED            : out    std_logic
		);
	end component;
	
	COMPONENT SwitchingSync
	Generic (CLK_DIV : integer);
	PORT(
		CLK : IN  std_logic;
		RESET : IN  std_logic;
		SETTING : IN  std_logic_vector(1 downto 0);
		SYNC_OUT : OUT  std_logic;
		SYNC_PULSE_IN : IN  std_logic
		);
	END COMPONENT;

	COMPONENT Sweep
	PORT(
		CLK : IN std_logic;
		RESET : IN std_logic;
		NPOINTS : IN std_logic_vector(12 downto 0);
		CONFIG_DATA : IN std_logic_vector(111 downto 0);
		SAMPLING_DONE : IN std_logic;
		MAX2871_DEF_4 : IN std_logic_vector(31 downto 0);
		MAX2871_DEF_3 : IN std_logic_vector(31 downto 0);
		MAX2871_DEF_1 : IN std_logic_vector(31 downto 0);
		MAX2871_DEF_0 : IN std_logic_vector(31 downto 0);
		PLL_RELOAD_DONE : IN std_logic;
		PLL_LOCKED : IN std_logic;
		SETTLING_TIME : IN std_logic_vector(15 downto 0);          
		CONFIG_ADDRESS : OUT std_logic_vector(12 downto 0);
		START_SAMPLING : OUT std_logic;
		PORT_SELECT : OUT std_logic;
		SOURCE_REG_4 : OUT std_logic_vector(31 downto 0);
		SOURCE_REG_3 : OUT std_logic_vector(31 downto 0);
		SOURCE_REG_1 : OUT std_logic_vector(31 downto 0);
		SOURCE_REG_0 : OUT std_logic_vector(31 downto 0);
		LO_REG_4 : OUT std_logic_vector(31 downto 0);
		LO_REG_3 : OUT std_logic_vector(31 downto 0);
		LO_REG_1 : OUT std_logic_vector(31 downto 0);
		LO_REG_0 : OUT std_logic_vector(31 downto 0);
		RELOAD_PLL_REGS : OUT std_logic;
		ATTENUATOR : OUT std_logic_vector(6 downto 0);
		SOURCE_FILTER : OUT std_logic_vector(1 downto 0)
		);
	END COMPONENT;
	COMPONENT Sampling
	Generic(CLK_DIV : integer;
		CLK_FREQ : integer;
		IF_FREQ : integer;
		CLK_CYCLES_PRE_DONE : integer);
	PORT(
		CLK : IN std_logic;
		RESET : IN std_logic;
		PORT1 : IN std_logic_vector(15 downto 0);
		PORT2 : IN std_logic_vector(15 downto 0);
		REF : IN std_logic_vector(15 downto 0);
		NEW_SAMPLE : IN std_logic;
		START : IN std_logic;
		SAMPLES : IN std_logic_vector(16 downto 0);          
		ADC_START : OUT std_logic;
		DONE : OUT std_logic;
		PRE_DONE : OUT std_logic;
		PORT1_I : OUT std_logic_vector(47 downto 0);
		PORT1_Q : OUT std_logic_vector(47 downto 0);
		PORT2_I : OUT std_logic_vector(47 downto 0);
		PORT2_Q : OUT std_logic_vector(47 downto 0);
		REF_I : OUT std_logic_vector(47 downto 0);
		REF_Q : OUT std_logic_vector(47 downto 0)
		);
	END COMPONENT;
	COMPONENT MCP33131
	Generic(CLK_DIV : integer;
			CONVCYCLES : integer);
	PORT(
		CLK : IN std_logic;
		RESET : IN std_logic;
		START : IN std_logic;
		SDO : IN std_logic;          
		READY : OUT std_logic;
		DATA : OUT std_logic_vector(15 downto 0);
		CONVSTART : OUT std_logic;
		SCLK : OUT std_logic
		);
	END COMPONENT;
	COMPONENT MAX2871
	Generic (CLK_DIV : integer);
	PORT(
		CLK : IN std_logic;
		RESET : IN std_logic;
		REG4 : IN std_logic_vector(31 downto 0);
		REG3 : IN std_logic_vector(31 downto 0);
		REG1 : IN std_logic_vector(31 downto 0);
		REG0 : IN std_logic_vector(31 downto 0);
		RELOAD : IN std_logic;          
		CLK_OUT : OUT std_logic;
		MOSI : OUT std_logic;
		LE : OUT std_logic;
		DONE : OUT std_logic
		);
	END COMPONENT;
	COMPONENT SPICommands
	PORT(
		CLK : IN std_logic;
		RESET : IN std_logic;
		SCLK : IN std_logic;
		MOSI : IN std_logic;
		NSS : IN std_logic;
		NEW_SAMPLING_DATA : IN std_logic;
		SAMPLING_RESULT : IN std_logic_vector(287 downto 0);
		SOURCE_UNLOCKED : IN std_logic;
		LO_UNLOCKED : IN std_logic;          
		MISO : OUT std_logic;
		MAX2871_DEF_4 : OUT std_logic_vector(31 downto 0);
		MAX2871_DEF_3 : OUT std_logic_vector(31 downto 0);
		MAX2871_DEF_1 : OUT std_logic_vector(31 downto 0);
		MAX2871_DEF_0 : OUT std_logic_vector(31 downto 0);
		SWEEP_DATA : OUT std_logic_vector(111 downto 0);
		SWEEP_ADDRESS : OUT std_logic_vector(12 downto 0);
		SWEEP_WRITE : OUT std_logic_vector(0 to 0);
		SWEEP_POINTS : OUT std_logic_vector(12 downto 0);
		NSAMPLES : OUT std_logic_vector(16 downto 0);
		SETTLING_TIME : out STD_LOGIC_VECTOR (15 downto 0);
		PORT1_EN : out STD_LOGIC;
		PORT2_EN : out STD_LOGIC;
		REF_EN : out STD_LOGIC;
		AMP_SHDN : out STD_LOGIC;
		SOURCE_RF_EN : out STD_LOGIC;
		LO_RF_EN : out STD_LOGIC;
		LEDS : out STD_LOGIC_VECTOR(2 downto 0);
		SYNC_SETTING : out STD_LOGIC_VECTOR(1 downto 0);
		INTERRUPT_ASSERTED : OUT std_logic
		);
	END COMPONENT;
	
	COMPONENT SweepConfigMem
	PORT (
		clka : IN STD_LOGIC;
		ena : IN STD_LOGIC;
		wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
		addra : IN STD_LOGIC_VECTOR(12 DOWNTO 0);
		dina : IN STD_LOGIC_VECTOR(111 DOWNTO 0);
		clkb : IN STD_LOGIC;
		addrb : IN STD_LOGIC_VECTOR(12 DOWNTO 0);
		doutb : OUT STD_LOGIC_VECTOR(111 DOWNTO 0)
		);
	END COMPONENT;
	
	signal clk160 : std_logic;
	signal clk_locked : std_logic;
	
	-- PLL signals
	signal source_reg_4 : std_logic_vector(31 downto 0);
	signal source_reg_3 : std_logic_vector(31 downto 0);
	signal source_reg_1 : std_logic_vector(31 downto 0);
	signal source_reg_0 : std_logic_vector(31 downto 0);
	signal lo_reg_4 : std_logic_vector(31 downto 0);
	signal lo_reg_3 : std_logic_vector(31 downto 0);
	signal lo_reg_1 : std_logic_vector(31 downto 0);
	signal lo_reg_0 : std_logic_vector(31 downto 0);
	signal reload_plls : std_logic;
	signal source_reloaded : std_logic;
	signal lo_reloaded : std_logic;
	signal plls_reloaded : std_logic;
	signal plls_locked : std_logic;
	
	-- ADC signals
	signal adc_trigger_sample : std_logic;
	signal adc_port1_ready : std_logic;
	signal adc_port1_data : std_logic_vector(15 downto 0);
	signal adc_port2_data : std_logic_vector(15 downto 0);
	signal adc_ref_data : std_logic_vector(15 downto 0);
	
	-- Sampling signals
	signal sampling_done : std_logic;
	signal sampling_start : std_logic;
	signal sampling_samples : std_logic_vector(16 downto 0);
	signal sampling_result : std_logic_vector(287 downto 0);
	
	-- Sweep signals
	signal sweep_points : std_logic_vector(12 downto 0);
	signal sweep_config_data : std_logic_vector(111 downto 0);
	signal sweep_config_address : std_logic_vector(12 downto 0);
	signal source_filter : std_logic_vector(1 downto 0);
	signal sweep_port_select : std_logic;
	
	signal sweep_config_write_address : std_logic_vector(12 downto 0);
	signal sweep_config_write_data : std_logic_vector(111 downto 0);
	signal sweep_config_write : std_logic_vector(0 downto 0);
	
	-- Configuration signals
	signal settling_time : std_logic_vector(15 downto 0);
	signal def_reg_4 : std_logic_vector(31 downto 0);
	signal def_reg_3 : std_logic_vector(31 downto 0);
	signal def_reg_1 : std_logic_vector(31 downto 0);
	signal def_reg_0 : std_logic_vector(31 downto 0);
	signal user_leds : std_logic_vector(2 downto 0);
	signal sync_setting : std_logic_vector(1 downto 0);
	
	-- PLL/SPI internal mux
	signal fpga_select : std_logic;
	signal fpga_source_SCK : std_logic;
	signal fpga_source_MOSI : std_logic;
	signal fpga_source_LE : std_logic;
	signal fpga_LO1_SCK : std_logic;
	signal fpga_LO1_MOSI : std_logic;
	signal fpga_LO1_LE : std_logic;
	signal fpga_miso : std_logic;
begin

	-- TODO assign proper signals
	LO1_CE <= '1';
	SOURCE_CE <= '1';
	BAND_SELECT <= '0';
	SDA <= 'Z';
	SCL <= 'Z';

	-- Reference CLK LED
	LEDS(0) <= user_leds(2);
	-- Lock status of PLLs
	LEDS(1) <= clk_locked;
	LEDS(2) <= SOURCE_LD;
	LEDS(3) <= LO1_LD;
	-- Sweep and active port
	PORT_SELECT <= sweep_port_select;
	LEDS(4) <= not (MCU_AUX3 and sweep_port_select);
	LEDS(5) <= not (MCU_AUX3 and not sweep_port_select);
	-- Uncommitted LEDs
	LEDS(7 downto 6) <= user_leds(1 downto 0);	

	MainCLK : PLL
	port map(
		-- Clock in ports
		CLK_IN1 => CLK,
		-- Clock out ports
		CLK_OUT1 => clk160,
		-- Status and control signals
		RESET  => '0',
		LOCKED => clk_locked
	);
	
	Sync: SwitchingSync
	GENERIC MAP (CLK_DIV => 160)
	PORT MAP (
		CLK => clk160,
		RESET => RESET,
		SETTING => sync_setting,
		SYNC_OUT => SWITCHING_SYNC,
		SYNC_PULSE_IN => '0' -- TODO leave ADCs running and connect to ADC trigger
	);

	Source: MAX2871
	GENERIC MAP(CLK_DIV => 10)
	PORT MAP(
		CLK => clk160,
		RESET => RESET,
		REG4 => source_reg_4,
		REG3 => source_reg_3,
		REG1 => source_reg_1,
		REG0 => source_reg_0,
		RELOAD => reload_plls,
		CLK_OUT => fpga_source_SCK,
		MOSI => fpga_source_MOSI,
		LE => fpga_source_LE,
		DONE => source_reloaded
	);
	LO1: MAX2871
	GENERIC MAP(CLK_DIV => 10)
	PORT MAP(
		CLK => clk160,
		RESET => RESET,
		REG4 => lo_reg_4,
		REG3 => lo_reg_3,
		REG1 => lo_reg_1,
		REG0 => lo_reg_0,
		RELOAD => reload_plls,
		CLK_OUT => fpga_LO1_SCK,
		MOSI => fpga_LO1_MOSI,
		LE => fpga_LO1_LE,
		DONE => lo_reloaded
	);
	plls_reloaded <= source_reloaded and lo_reloaded;
	plls_locked <= SOURCE_LD and LO1_LD;

	Port1ADC: MCP33131
	GENERIC MAP(CLK_DIV => 2,
				CONVCYCLES => 114)
	PORT MAP(
		CLK => clk160,
		RESET => RESET,
		START => adc_trigger_sample,
		READY => adc_port1_ready,
		DATA => adc_port1_data,
		SDO => PORT1_SDO,
		CONVSTART => PORT1_CONVSTART,
		SCLK => PORT1_SCLK
	);
	Port2ADC: MCP33131
	GENERIC MAP(CLK_DIV => 2,
				CONVCYCLES => 114)
	PORT MAP(
		CLK => clk160,
		RESET => RESET,
		START => adc_trigger_sample,
		READY => open, -- synchronous ADCs, ready indicated by port 1 ADC
		DATA => adc_port2_data,
		SDO => PORT2_SDO,
		CONVSTART => PORT2_CONVSTART,
		SCLK => PORT2_SCLK
	);
	RefADC: MCP33131
	GENERIC MAP(CLK_DIV => 2,
				CONVCYCLES => 114)
	PORT MAP(
		CLK => clk160,
		RESET => RESET,
		START => adc_trigger_sample,
		READY => open, -- synchronous ADCs, ready indicated by port 1 ADC
		DATA => adc_ref_data,
		SDO => REF_SDO,
		CONVSTART => REF_CONVSTART,
		SCLK => REF_SCLK
	);
	
	Sampler: Sampling
	GENERIC MAP(CLK_DIV => 165,
			CLK_FREQ => 160000000,
			IF_FREQ => 250000,
			CLK_CYCLES_PRE_DONE => 0)
	PORT MAP(
		CLK => clk160,
		RESET => RESET,
		PORT1 => adc_port1_data,
		PORT2 => adc_port2_data,
		REF => adc_ref_data,
		ADC_START => adc_trigger_sample,
		NEW_SAMPLE => adc_port1_ready,
		DONE => sampling_done,
		PRE_DONE => open,
		START => sampling_start,
		SAMPLES => sampling_samples,
		PORT1_I => sampling_result(287 downto 240),
		PORT1_Q => sampling_result(239 downto 192),
		PORT2_I => sampling_result(191 downto 144),
		PORT2_Q => sampling_result(143 downto 96),
		REF_I => sampling_result(95 downto 48),
		REF_Q => sampling_result(47 downto 0)
	);

	SweepModule: Sweep PORT MAP(
		CLK => clk160,
		RESET => MCU_AUX3,
		NPOINTS => sweep_points,
		CONFIG_ADDRESS => sweep_config_address,
		CONFIG_DATA => sweep_config_data,
		SAMPLING_DONE => sampling_done,
		START_SAMPLING => sampling_start,
		PORT_SELECT => sweep_port_select,
		MAX2871_DEF_4 => def_reg_4,
		MAX2871_DEF_3 => def_reg_3,
		MAX2871_DEF_1 => def_reg_1,
		MAX2871_DEF_0 => def_reg_0,
		SOURCE_REG_4 => source_reg_4,
		SOURCE_REG_3 => source_reg_3,
		SOURCE_REG_1 => source_reg_1,
		SOURCE_REG_0 => source_reg_0,
		LO_REG_4 => lo_reg_4,
		LO_REG_3 => lo_reg_3,
		LO_REG_1 => lo_reg_1,
		LO_REG_0 => lo_reg_0,
		RELOAD_PLL_REGS => reload_plls,
		PLL_RELOAD_DONE => plls_reloaded,
		PLL_LOCKED => plls_locked,
		ATTENUATOR => ATTENUATION,
		SOURCE_FILTER => source_filter,
		SETTLING_TIME => settling_time
	);
	
	-- Source filter mapping
	FILT_IN_C1 <= '0' when source_filter = "00" or source_filter = "10" else '1';
	FILT_IN_C2 <= '0' when source_filter = "11" or source_filter = "10" else '1';
	FILT_OUT_C1 <= '0' when source_filter = "00" or source_filter = "10" else '1';
	FILT_OUT_C2 <= '0' when source_filter = "00" or source_filter = "01" else '1';
	
	-- PLL/SPI mux
	-- only select FPGA SPI slave when both AUX1 and AUX2 are low
	fpga_select <= MCU_NSS when MCU_AUX1 = '0' and MCU_AUX2 = '0' else '1';
	-- direct connection between MCU and SOURCE when AUX1 is high
	SOURCE_CLK <= MCU_SCK when MCU_AUX1 = '1' else fpga_source_SCK;
	SOURCE_MOSI <= MCU_MOSI when MCU_AUX1 = '1' else fpga_source_MOSI;
	SOURCE_LE <= MCU_NSS when MCU_AUX1 = '1' else fpga_source_LE;
	-- direct connection between MCU and LO1 when AUX2 is high
	LO1_CLK <= MCU_SCK when MCU_AUX2 = '1' else fpga_LO1_SCK;
	LO1_MOSI <= MCU_MOSI when MCU_AUX2 = '1' else fpga_LO1_MOSI;
	LO1_LE <= MCU_NSS when MCU_AUX2 = '1' else fpga_LO1_LE;
	-- select MISO source
	MCU_MISO <= SOURCE_MUX when MCU_AUX1 = '1' else LO1_MUX when MCU_AUX2 = '1' else fpga_miso;

	SPI: SPICommands PORT MAP(
		CLK => clk160,
		RESET => RESET,
		SCLK => MCU_SCK,
		MOSI => MCU_MOSI,
		MISO => fpga_miso,
		NSS => fpga_select,
		NEW_SAMPLING_DATA => sampling_done,
		SAMPLING_RESULT => sampling_result,
		SOURCE_UNLOCKED => SOURCE_LD, -- TODO invert
		LO_UNLOCKED => LO1_LD, -- TODO invert
		MAX2871_DEF_4 => def_reg_4,
		MAX2871_DEF_3 => def_reg_3,
		MAX2871_DEF_1 => def_reg_1,
		MAX2871_DEF_0 => def_reg_0,
		SWEEP_DATA => sweep_config_write_data,
		SWEEP_ADDRESS => sweep_config_write_address,
		SWEEP_WRITE => sweep_config_write,
		SWEEP_POINTS => sweep_points,
		NSAMPLES => sampling_samples,
		SETTLING_TIME => settling_time,
		PORT1_EN => PORT1_MIX_EN,
		PORT2_EN => PORT2_MIX_EN,
		REF_EN => REF_MIX_EN,
		AMP_SHDN => AMP_PWDN,
		SOURCE_RF_EN => SOURCE_RF_EN,
		LO_RF_EN => LO1_RF_EN,
		LEDS => user_leds,
		SYNC_SETTING => sync_setting,
		INTERRUPT_ASSERTED => MCU_INTR
	);
	
	ConfigMem : SweepConfigMem
	PORT MAP (
		clka => clk160,
		ena => '1',
		wea => sweep_config_write,
		addra => sweep_config_write_address,
		dina => sweep_config_write_data,
		clkb => clk160,
		addrb => sweep_config_address,
		doutb => sweep_config_data
	);

end Behavioral;

