----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    16:59:45 05/05/2020 
-- Design Name: 
-- Module Name:    MAX2871 - Behavioral 
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

entity MAX2871 is
	 Generic (CLK_DIV : integer);
    Port ( CLK : in  STD_LOGIC;
           RESET : in  STD_LOGIC;
           REG4 : in  STD_LOGIC_VECTOR (31 downto 0);
           REG3 : in  STD_LOGIC_VECTOR (31 downto 0);
           REG1 : in  STD_LOGIC_VECTOR (31 downto 0);
           REG0 : in  STD_LOGIC_VECTOR (31 downto 0);
           RELOAD : in  STD_LOGIC;
           CLK_OUT : out  STD_LOGIC;
           MOSI : out  STD_LOGIC;
           LE : out  STD_LOGIC;
           DONE : out  STD_LOGIC);
end MAX2871;

architecture Behavioral of MAX2871 is
	signal clk_cnt : integer range 0 to (CLK_DIV/2)-1;
	
begin


end Behavioral;

