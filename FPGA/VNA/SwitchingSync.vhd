----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    14:47:06 05/10/2020 
-- Design Name: 
-- Module Name:    SwitchingSync - Behavioral 
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

entity SwitchingSync is
	 Generic (CLK_DIV : integer);
    Port ( CLK : in  STD_LOGIC;
           RESET : in  STD_LOGIC;
           SETTING : in  STD_LOGIC_VECTOR (1 downto 0);
           SYNC_OUT : out  STD_LOGIC;
           SYNC_PULSE_IN : in  STD_LOGIC);
end SwitchingSync;

architecture Behavioral of SwitchingSync is
	signal clk_cnt : integer range 0 to (CLK_DIV/2)-1;
	signal sync : std_logic;
begin

	process(CLK, RESET)
	begin
		if rising_edge(CLK) then
			if RESET = '1' then
				clk_cnt <= 0;
				sync <= 'Z';
			else
				case SETTING is
					when "01" =>
						-- Square wave output
						if clk_cnt < (CLK_DIV/2) - 1 then
							clk_cnt <= clk_cnt + 1;
						else
							clk_cnt <= 0;
							if sync = '0' then
								sync <= '1';
								SYNC_OUT <= '1';
							else
								sync <= '0';
								SYNC_OUT <= '0';
							end if;
						end if;
					when "10" =>
						-- Square wave output synchronized to sync_pulse_in
						if sync = '0' and sync_pulse_in = '1' then
							sync <= '1';
							SYNC_OUT <= '1';
							clk_cnt <= 0;
						elsif clk_cnt < (CLK_DIV/2) - 1 then
							clk_cnt <= clk_cnt + 1;
						else
							sync <= '0';
							SYNC_OUT <= '0';
						end if;
					when others => SYNC_OUT <= 'Z';
				end case;
			end if;
		end if;
	end process;

end Behavioral;

