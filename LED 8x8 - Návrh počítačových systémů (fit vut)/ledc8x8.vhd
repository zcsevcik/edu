-- ledc8x8.vhd: Rizeni maticoveho displeje BM(HD)-10EG88MD
--              projekt c. 1 pro predmet INP
-- Date:        2012/10/07
-- Author(s):   Radek Sevcik (xsevci44@stud.fit.vutbr.cz)

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity ledc8x8 is
  port (
    -- Input signals
    SMCLK       : in  std_logic;
    RESET       : in  std_logic;
    
    -- Output signals
    ROW         : out std_logic_vector(7 downto 0);
    LED         : out std_logic_vector(0 to 7)
  );
end entity ledc8x8;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of ledc8x8 is

  signal clock_cnt_ce  : std_logic;
  signal clock_cnt     : std_logic_vector(7 downto 0);
  signal row_cnt       : std_logic_vector(7 downto 0);

begin
  -- -------------------------------------------------------
  clock_cnt_proc : process(SMCLK, RESET)
  begin
    -- on reset, zero-fill clock_cnt
    -- on clk, increment clock_cnt

    if (RESET = '1') then
      clock_cnt <= (others => '0');
    elsif (rising_edge(SMCLK)) then
      clock_cnt <= std_logic_vector(unsigned(clock_cnt) + 1);
    end if;
  end process clock_cnt_proc;

  -- -------------------------------------------------------
  -- FIXME: error occurs if the command moves to clock_cnt_proc
  --        (164) Parse error, unexpected WHEN, expecting SEMICOLON.
  -- clock_cnt_ce = SMCLK/256
  clock_cnt_ce <= '1' when (clock_cnt = x"FF") else '0';

  -- -------------------------------------------------------
  row_cnt_proc : process(SMCLK, RESET, clock_cnt_ce)
  begin
    -- row active in log.0, i.e. ~(1 << row)
    -- next_row = row(0) & row(7 downto 1), i.e. row ror 1
    -- on reset, activate row 1 (~80h)

    if (RESET = '1') then
      row_cnt <= not(x"80");
    elsif (rising_edge(SMCLK) and clock_cnt_ce = '1') then
      row_cnt <= std_logic_vector(unsigned(row_cnt) ror 1);
    end if;
  end process row_cnt_proc;

  -- -------------------------------------------------------
  dec_proc : process(row_cnt)
  begin
    -- FIXME: warning occurs when x"7F" is replaced by not(x"80") etc.
    --        (817) Choice 'not' is not a locally static expression.

    case row_cnt is
      when x"FE"  =>  LED <= "11100000";
      when x"FD"  =>  LED <= "10010011";
      when x"FB"  =>  LED <= "10010100";
      when x"F7"  =>  LED <= "11100100";
      when x"EF"  =>  LED <= "10010010";
      when x"DF"  =>  LED <= "10001001";
      when x"BF"  =>  LED <= "00000001";
      when x"7F"  =>  LED <= "00000110";
      when others =>  LED <= "00000000";
    end case;
  end process dec_proc; 

  -- -------------------------------------------------------
  ROW <= row_cnt;

  -- -------------------------------------------------------

end architecture behavioral;
