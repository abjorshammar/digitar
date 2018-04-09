local _M = {}

function _M.run(display)
  --wifi.setmode(wifi.STATIONAP)
  --wifi.ap.config({ssid="MyPersonalSSID", auth=wifi.OPEN})

  enduser_setup.start(
    function()
      display:print("SSID:", wifi.sta.getconfig(true).ssid or "Not set")
      --print("Connected to wifi as:" .. wifi.sta.getip())
    end,
    function(err, str)
      display:print("Bad stuff", "")
      --print("enduser_setup: Err #" .. err .. ": " .. str)
    end,
    print -- Lua print function can serve as the debug callback
   );
end

return _M