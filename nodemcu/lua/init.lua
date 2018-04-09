local wifi_setup = require "wifi_setup"
local oled = require "oled"

-- Hardware: 
--   ESP-12E Devkit
--   4 pin I2C OLED 128x64 Display Module
-- Connections:
--   ESP  --  OLED
--   3v3  --  VCC
--   GND  --  GND
--   D1   --  SDA
--   D2   --  SCL

-- Variables 
local sda = 1 -- SDA Pin
local scl = 2 -- SCL Pin

local display = oled:new(sda, scl)

display:print("Finding Wi-Fi", "")

wifi_setup.run(display)
