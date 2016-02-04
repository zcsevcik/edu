-- cpu.vhd:   Simple 8-bit CPU (BrainFuck interpreter)
-- Date:      2012/10/27
-- Author(s): Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
  -- radic (Moore FSM)
  type fsm_state is (
      I_INCPTR, I_DECPTR, I_INC0, I_INC1, I_DEC0, I_DEC1,
      I_WHILE0, I_WHILE1, I_WHILE2, I_WHILE3,
      I_ENDWHILE0, I_ENDWHILE1, I_ENDWHILE2, I_ENDWHILE3, I_ENDWHILE4,
      I_PUTC0, I_PUTC1, I_GETC0, I_GETC1, I_NOP, I_EOF,
      S_IDLE, S_FETCH, S_DECODE, S_HALT);
  signal pstate, nstate : fsm_state;

  -- ram logika
  signal data_zero   : std_logic;
  signal data_mx_sel : std_logic_vector(1 downto 0);

  -- program counter, velikost 4kB
  signal pc_reg : std_logic_vector(11 downto 0);
  signal pc_inc : std_logic;
  signal pc_dec : std_logic;
  
  -- citac vnoreni cyklu
  signal cnt_reg  : std_logic_vector(9 downto 0);
  signal cnt_inc  : std_logic;
  signal cnt_dec  : std_logic;
  signal cnt_zero : std_logic;
  signal cnt_one  : std_logic;
  
  -- ukazatel bunky, velikost 1kB
  signal ptr_reg  : std_logic_vector(9 downto 0);
  signal ptr_inc  : std_logic;
  signal ptr_dec  : std_logic;

begin   
  -- --------------------------------------------------------------------------
  DATA_WDATA <= DATA_RDATA + 1 when data_mx_sel = "00" else
            DATA_RDATA - 1 when data_mx_sel = "01" else
            IN_DATA;
           
  -- --------------------------------------------------------------------------
  data_zero <= '1' when (DATA_RDATA = x"00") else '0';
           
  -- --------------------------------------------------------------------------
  cnt_zero <= '1' when (cnt_reg = "0000000000") else '0';
           
  -- --------------------------------------------------------------------------
  DATA_ADDR <= ptr_reg;
           
  -- --------------------------------------------------------------------------
  CODE_ADDR <= pc_reg;      

  -- --------------------------------------------------------------------------
  fsm_pstate: process(RESET, CLK, EN)
  begin
    if (RESET = '1') then
      pstate <= S_IDLE;
    elsif (rising_edge(CLK)) then
      if (EN = '1') then
        pstate <= nstate;
      end if;
    end if;
  end process;

  -- --------------------------------------------------------------------------
  fsm_nstate: process(pstate, CODE_DATA, OUT_BUSY, IN_VLD, cnt_zero, data_zero)
  begin
    nstate <= S_IDLE;
   
    case pstate is
    -- ------------------------------------------------------------------------
    when S_IDLE =>
      nstate <= S_FETCH;

    -- ------------------------------------------------------------------------
    when S_FETCH =>
      nstate <= S_DECODE;

    -- ------------------------------------------------------------------------
    when S_DECODE =>
      case CODE_DATA is
      -- ----------------------------------------------------------------------
      when x"3E" =>
        nstate <= I_INCPTR;
    
      -- ----------------------------------------------------------------------
      when x"3C" =>
        nstate <= I_DECPTR;
      
      -- ----------------------------------------------------------------------
      when x"2B" =>
        nstate <= I_INC0;
      
      -- ----------------------------------------------------------------------
      when x"2D" =>
        nstate <= I_DEC0;
      
      -- ----------------------------------------------------------------------
      when x"5B" =>
        nstate <= I_WHILE0;
      
      -- ----------------------------------------------------------------------
      when x"5D" =>
        nstate <= I_ENDWHILE0;
      
      -- ----------------------------------------------------------------------
      when x"2E" =>
        nstate <= I_PUTC0;
      
      -- ----------------------------------------------------------------------
      when x"2C" =>
        nstate <= I_GETC0;
      
      -- ----------------------------------------------------------------------
      when x"00" =>
        nstate <= I_EOF;
      
      -- ----------------------------------------------------------------------
      when others =>
        nstate <= I_NOP;
      
      -- ----------------------------------------------------------------------
      end case;

    -- ------------------------------------------------------------------------
    when I_EOF =>
      nstate <= S_HALT;
    
    -- ------------------------------------------------------------------------
    when I_NOP =>
      nstate <= S_FETCH;
    
    -- ------------------------------------------------------------------------
    when I_INC0 =>
      nstate <= I_INC1;
    
    -- ------------------------------------------------------------------------
    when I_INC1 =>
      nstate <= S_FETCH;
    
    -- ------------------------------------------------------------------------
    when I_DEC0 =>
      nstate <= I_DEC1;
    
    -- ------------------------------------------------------------------------
    when I_DEC1 =>
      nstate <= S_FETCH;
    
    -- ------------------------------------------------------------------------
    when I_INCPTR =>
      nstate <= S_FETCH;
    
    -- ------------------------------------------------------------------------
    when I_DECPTR =>
      nstate <= S_FETCH;
    
    -- ------------------------------------------------------------------------
    when I_GETC0 =>
      if IN_VLD = '1' then
        nstate <= I_GETC1;
      else
        nstate <= I_GETC0;
      end if;
    
    -- ------------------------------------------------------------------------
    when I_GETC1 =>
      nstate <= S_FETCH;
    
    -- ------------------------------------------------------------------------
    when I_PUTC0 =>
      if OUT_BUSY = '0' then
        nstate <= I_PUTC1;
      else
        nstate <= I_PUTC0;
      end if;
    
    -- ------------------------------------------------------------------------
    when I_PUTC1 =>
      nstate <= S_FETCH;
    
    -- ------------------------------------------------------------------------
    when I_WHILE0 =>
      nstate <= I_WHILE1;
    
    -- ------------------------------------------------------------------------
    when I_WHILE1 =>
      if data_zero /= '1' then
        nstate <= I_WHILE2;
      else
        nstate <= S_FETCH;
      end if;
    
    -- ------------------------------------------------------------------------
    when I_WHILE2 =>
      if cnt_zero /= '1' then
        nstate <= I_WHILE3;
      else
        nstate <= S_FETCH;
      end if;
    
    -- ------------------------------------------------------------------------
    when I_WHILE3 =>
      nstate <= I_WHILE2;
    
    -- ------------------------------------------------------------------------
    when I_ENDWHILE0 =>
      nstate <= I_ENDWHILE1;
    
    -- ------------------------------------------------------------------------
    when I_ENDWHILE1 =>
      if data_zero /= '1' then
        nstate <= I_ENDWHILE2;
      else
        nstate <= S_FETCH;
      end if;
    
    -- ------------------------------------------------------------------------
    when I_ENDWHILE2 =>
      if cnt_zero /= '1' then
        nstate <= I_ENDWHILE3;
      else
        nstate <= S_FETCH;
      end if;
    
    -- ------------------------------------------------------------------------
    when I_ENDWHILE3 =>
      nstate <= I_ENDWHILE4;
    
    -- ------------------------------------------------------------------------
    when I_ENDWHILE4 =>
      nstate <= I_ENDWHILE2;
    
    -- ------------------------------------------------------------------------
    when S_HALT =>
      nstate <= S_HALT;
    
    -- ------------------------------------------------------------------------
    when others =>
      nstate <= S_IDLE;
    
    -- ------------------------------------------------------------------------
    end case;
   
  end process;

  -- --------------------------------------------------------------------------
  fsm_ologic: process(pstate, CODE_DATA, cnt_zero, data_zero)
  begin
    pc_inc <= '0';
    pc_dec <= '0';
    cnt_inc <= '0';
    cnt_dec <= '0';
    cnt_one <= '0';
    ptr_inc <= '0';
    ptr_dec <= '0';
    
    CODE_EN <= '0';
    DATA_EN <= '0';
    
    DATA_RDWR <= '0';

    IN_REQ <= '0';
    OUT_WE <= '0';
  
    case pstate is
    -- ------------------------------------------------------------------------
    when I_INCPTR =>
      pc_inc <= '1';
      ptr_inc <= '1';
       
    -- ------------------------------------------------------------------------
    when I_DECPTR =>
      pc_inc <= '1';
      ptr_dec <= '1';
       
    -- ------------------------------------------------------------------------
    when I_INC0 =>
      DATA_RDWR <= '0';
      DATA_EN <= '1';
    
    -- ------------------------------------------------------------------------
    when I_INC1 =>
      pc_inc <= '1';
      data_mx_sel <= "00";
      DATA_RDWR <= '1';
      DATA_EN <= '1';
       
    -- ------------------------------------------------------------------------
    when I_DEC0 =>
      DATA_RDWR <= '0';
      DATA_EN <= '1';
     
    -- ------------------------------------------------------------------------
    when I_DEC1 =>
      pc_inc <= '1';
      data_mx_sel <= "01";
      DATA_RDWR <= '1';
      DATA_EN <= '1';
       
    -- ------------------------------------------------------------------------
    when I_GETC0 =>
      IN_REQ <= '1';
             
    -- ------------------------------------------------------------------------
    when I_GETC1 =>
      pc_inc <= '1';
      data_mx_sel <= "11";
      IN_REQ <= '1';
      DATA_RDWR <= '1';
      DATA_EN <= '1';
             
    -- ------------------------------------------------------------------------
    when I_PUTC0 =>
      null;
             
    -- ------------------------------------------------------------------------
    when I_PUTC1 =>
      OUT_DATA <= DATA_RDATA;
      OUT_WE <= '1';
      pc_inc <= '1';
             
    -- ------------------------------------------------------------------------
    when I_NOP =>
      pc_inc <= '1';
             
    -- ------------------------------------------------------------------------
    when I_WHILE0 =>
      pc_inc <= '1';
      DATA_RDWR <= '0';
      DATA_EN <= '1';
    
    -- ------------------------------------------------------------------------
    when I_WHILE1 =>
      if data_zero = '1' then
        cnt_one <= '1';
      end if;
    
    -- ------------------------------------------------------------------------
    when I_WHILE2 =>
      CODE_EN <= '1';
    
    -- ------------------------------------------------------------------------
    when I_WHILE3 =>
      pc_inc <= '1';
      if (CODE_DATA = x"5B") then
        cnt_inc <= '1';
      elsif (CODE_DATA = x"5D") then
        cnt_dec <= '1';
      end if;
    
    -- ------------------------------------------------------------------------
    when I_ENDWHILE0 =>
      DATA_RDWR <= '0';
      DATA_EN <= '1';
    
    -- ------------------------------------------------------------------------
    when I_ENDWHILE1 =>
      if data_zero = '1' then
        pc_inc <= '1';
      else
        cnt_one <= '1';
        pc_dec <= '1';
      end if;
    
    -- ------------------------------------------------------------------------
    when I_ENDWHILE2 =>
      CODE_EN <= '1';
    
    -- ------------------------------------------------------------------------
    when I_ENDWHILE3 =>
      if (CODE_DATA = x"5B") then
        cnt_dec <= '1';
      elsif (CODE_DATA = x"5D") then
        cnt_inc <= '1';
      end if;
     
    -- ------------------------------------------------------------------------
    when I_ENDWHILE4 =>
      if cnt_zero = '1' then
        pc_inc <= '1';
      else
        pc_dec <= '1';
      end if;
    
    -- ------------------------------------------------------------------------
    when S_FETCH =>
      CODE_EN <= '1';
    
    -- ------------------------------------------------------------------------
    when others =>
      null;
    
    -- ------------------------------------------------------------------------
    end case;
   
  end process;
      
  -- --------------------------------------------------------------------------
  pc_proc: process(RESET, CLK, pc_inc, pc_dec)
  begin
    if (RESET = '1') then
      pc_reg <= (others => '0');
    elsif (rising_edge(CLK)) then
      if (pc_inc = '1') then
        pc_reg <= pc_reg + 1;
      elsif (pc_dec = '1') then
        pc_reg <= pc_reg - 1;
      end if;
    end if;
  end process;
  
  -- --------------------------------------------------------------------------
  cnt_proc: process(RESET, CLK, cnt_inc, cnt_dec, cnt_one)
  begin
    if (RESET = '1') then
      cnt_reg <= (others => '0');
    elsif (rising_edge(CLK)) then
      if (cnt_inc = '1') then
        cnt_reg <= cnt_reg + 1;
      elsif (cnt_dec = '1') then
        cnt_reg <= cnt_reg - 1;
      elsif (cnt_one = '1') then
        cnt_reg <= (0 => '1', others => '0');
      end if;
    end if;
  end process;

  -- --------------------------------------------------------------------------
  ptr_proc: process(RESET, CLK, ptr_inc, ptr_dec)
  begin
    if (RESET = '1') then
      ptr_reg <= (others => '0');
    elsif (rising_edge(CLK)) then
      if (ptr_inc = '1') then
        ptr_reg <= ptr_reg + 1;
      elsif (ptr_dec = '1') then
        ptr_reg <= ptr_reg - 1;
      end if;
    end if;
  end process;

  -- --------------------------------------------------------------------------
end behavioral;

