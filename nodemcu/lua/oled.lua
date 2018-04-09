local _M = {}

function _M:new(sda, scl)
     local obj = {}
     local sla = 0x3C
     i2c.setup(0, sda, scl, i2c.SLOW)
     local display = u8g.sh1106_128x64_i2c(sla)
     display:setFont(u8g.font_6x10)
     display:setFontRefHeightExtendedText()
     display:setDefaultForegroundColor()
     display:setFontPosTop()
     obj.display = display
     setmetatable(obj, self)
     self.__index = self
     return obj
end

function _M:print(str1, str2)
   local display = self.display
   display:firstPage()
   repeat
     display:drawFrame(2,2,126,62)
     display:drawStr(5, 10, str1)
     display:drawStr(5, 20, str2)
     display:drawCircle(18, 47, 14)
   until display:nextPage() == false
end

return _M