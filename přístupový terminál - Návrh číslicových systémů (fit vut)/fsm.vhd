-- fsm.vhd: Finite State Machine
-- Author(s): Radek Sevcik (xsevci44@stud.fit.vutbr.cz)
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (PWD_CHECK, PWD_OK, PWD_ERR, MSG_ACCESS_GRANTED, MSG_ACCESS_DENIED, FINISH, PWD_NUM1_OK,
                    PWD1_NUM2_OK, PWD1_NUM3_OK, PWD1_NUM4_OK, PWD1_NUM5_OK, PWD1_NUM6_OK, PWD1_NUM7_OK, PWD1_NUM8_OK,
                    PWD2_NUM2_OK, PWD2_NUM3_OK, PWD2_NUM4_OK, PWD2_NUM5_OK, PWD2_NUM6_OK, PWD2_NUM7_OK, PWD2_NUM8_OK, PWD2_NUM9_OK, PWD2_NUM10_OK);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= PWD_CHECK;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD_CHECK =>
      next_state <= PWD_CHECK;
      if (KEY(9) = '1') then
         next_state <= PWD_NUM1_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD_NUM1_OK =>
      next_state <= PWD_NUM1_OK;
      if (KEY(3) = '1') then
         next_state <= PWD1_NUM2_OK;
      elsif (KEY(4) = '1') then
         next_state <= PWD2_NUM2_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD1_NUM2_OK =>
      next_state <= PWD1_NUM2_OK;
      if (KEY(4) = '1') then
         next_state <= PWD1_NUM3_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD1_NUM3_OK =>
      next_state <= PWD1_NUM3_OK;
      if (KEY(2) = '1') then
         next_state <= PWD1_NUM4_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD1_NUM4_OK =>
      next_state <= PWD1_NUM4_OK;
      if (KEY(5) = '1') then
         next_state <= PWD1_NUM5_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD1_NUM5_OK =>
      next_state <= PWD1_NUM5_OK;
      if (KEY(9) = '1') then
         next_state <= PWD1_NUM6_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD1_NUM6_OK =>
      next_state <= PWD1_NUM6_OK;
      if (KEY(9) = '1') then
         next_state <= PWD1_NUM7_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD1_NUM7_OK =>
      next_state <= PWD1_NUM7_OK;
      if (KEY(0) = '1') then
         next_state <= PWD1_NUM8_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD1_NUM8_OK =>
      next_state <= PWD1_NUM8_OK;
      if (KEY(7) = '1') then
         next_state <= PWD_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD2_NUM2_OK =>
      next_state <= PWD2_NUM2_OK;
      if (KEY(9) = '1') then
         next_state <= PWD2_NUM3_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD2_NUM3_OK =>
      next_state <= PWD2_NUM3_OK;
      if (KEY(4) = '1') then
         next_state <= PWD2_NUM4_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD2_NUM4_OK =>
      next_state <= PWD2_NUM4_OK;
      if (KEY(8) = '1') then
         next_state <= PWD2_NUM5_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD2_NUM5_OK =>
      next_state <= PWD2_NUM5_OK;
      if (KEY(1) = '1') then
         next_state <= PWD2_NUM6_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD2_NUM6_OK =>
      next_state <= PWD2_NUM6_OK;
      if (KEY(5) = '1') then
         next_state <= PWD2_NUM7_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD2_NUM7_OK =>
      next_state <= PWD2_NUM7_OK;
      if (KEY(8) = '1') then
         next_state <= PWD2_NUM8_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD2_NUM8_OK =>
      next_state <= PWD2_NUM8_OK;
      if (KEY(5) = '1') then
         next_state <= PWD2_NUM9_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD2_NUM9_OK =>
      next_state <= PWD2_NUM9_OK;
      if (KEY(1) = '1') then
         next_state <= PWD2_NUM10_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD2_NUM10_OK =>
      next_state <= PWD2_NUM10_OK;
      if (KEY(2) = '1') then
         next_state <= PWD_OK;
      elsif (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD_ERR =>
      next_state <= PWD_ERR;
      if (KEY(15) = '1') then
         next_state <= MSG_ACCESS_DENIED; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PWD_OK =>
      next_state <= PWD_OK;
      if (KEY(15) = '1') then
         next_state <= MSG_ACCESS_GRANTED; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= PWD_ERR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when MSG_ACCESS_GRANTED =>
      next_state <= MSG_ACCESS_GRANTED;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when MSG_ACCESS_DENIED =>
      next_state <= MSG_ACCESS_DENIED;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= PWD_CHECK; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= PWD_CHECK;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when MSG_ACCESS_GRANTED =>
      FSM_CNT_CE     <= '1';
      FSM_MX_MEM     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when MSG_ACCESS_DENIED =>
      FSM_CNT_CE     <= '1';
      FSM_MX_MEM     <= '0';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;

