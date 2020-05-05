----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    16:01:43 05/05/2020 
-- Design Name: 
-- Module Name:    MCP33131 - Behavioral 
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

entity MCP33131 is
	 Generic(CLK_DIV : integer;
				CONVCYCLES : integer);
    Port ( CLK : in  STD_LOGIC;
           RESET : in  STD_LOGIC;
           START : in  STD_LOGIC;
           READY : out  STD_LOGIC;
           DATA : out  STD_LOGIC_VECTOR (15 downto 0);
           SDO : in  STD_LOGIC;
           CONVSTART : out  STD_LOGIC;
           SCLK : out  STD_LOGIC);
end MCP33131;

architecture Behavioral of MCP33131 is
	signal conv_cnt : integer range 0 to CONVCYCLES-1;
	signal div_cnt : integer range 0 to (CLK_DIV/2)-1;
	signal sclk_phase : std_logic;
	signal adc_data : std_logic_vector(15 downto 0);
	signal conversion_active : std_logic;
	signal transmission_active : std_logic;
	signal new_result : std_logic;
begin

	SCLK <= sclk_phase;
	READY <= new_result;

	process(CLK, RESET)
	begin
		if(rising_edge(CLK)) then
			if(RESET = '1') then
				DATA <= (others => '0');
				new_result <= '0';
				CONVSTART <= '0';
				conv_cnt <= 0;
				div_cnt <= 0;
				conversion_active <= '0';
				transmission_active <= '0';
				adc_data <= "0000000000000001";
			else
				if(new_result = '1') then
					new_result <= '0';
				end if;
				if conversion_active = '0' and transmission_active = '0' and START = '1' then
					-- start new conversion
					conversion_active <= '1';
					CONVSTART <= '1';
					conv_cnt <= 0;
					adc_data <= "0000000000000001";
				elsif conversion_active = '1' then
					-- keep track of conversion time
					if(conv_cnt < CONVCYCLES-1) then
						conv_cnt <= conv_cnt + 1;
					else
						CONVSTART <= '0';
						conversion_active <= '0';
						transmission_active <= '1';
						div_cnt <= 0;
						sclk_phase <= '0';
					end if;
				elsif transmission_active = '1' then
					if(div_cnt < (CLK_DIV/2)-1) then
						div_cnt <= div_cnt + 1;
					else
						if(sclk_phase = '0') then
							sclk_phase <= '1';
						else
							sclk_phase <= '0';
							if(adc_data(15) = '0') then
								-- not the last bit yet
								adc_data <= adc_data(14 downto 0) & SDO;
							else
								-- last bit, move to output and indicate ready state
								DATA <= adc_data(14 downto 0) & SDO;
								new_result <= '1';
								transmission_active <= '0';
							end if;
						end if;
						div_cnt <= 0;
					end if;
				end if;
			end if;
		end if;
	end process;

end Behavioral;

