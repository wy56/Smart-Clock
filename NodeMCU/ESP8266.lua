--Settings
WIFI_STA_SSID = "WiFi_SSID"
WIFI_STA_PW = "WiFI_Password"
WIFI_AP_SSID = "SmartClock-11597"
WIFI_AP_PW = ""
NTP_SERVER_IP = "59.124.29.241"
NTP_SYNC_FEQ = 5
NTP_GMT = 8

--UART Setup
uart.setup(1, 9600, 8, uart.PARITY_NONE, uart.STOPBITS_1, 1) --UART For STM32
--Wi-Fi Setup
wifi_sta_setup(WIFI_STA_SSID, WIFI_STA_PW)
wifi_ap_setup(WIFI_AP_SSID, WIFI_AP_PW)
ip=wifi.ap.getip()
print(ip)

-- Create a server
-- and set 30s time out for a inactive client
srv = net.createServer(net.TCP, 30)
-- Server listen on 80
srv:listen(80, function(conn)
  conn:on("receive", receiver)
end)

--Automatic Sync NTP Time
mytimer2 = tmr.create()
mytimer2:register(30000, tmr.ALARM_SEMI , ntp_sync() ) --For 30 Sec
mytimer2:start()

---------------------------------------------------------------

function wifi_sta_setup(ssid, pwd)
    wifi.setmode(3)
    station_cfg={}
    station_cfg.ssid=ssid
    station_cfg.pwd=pwd
    station_cfg.save=false
    wifi.sta.config(station_cfg)
end

function wifi_ap_setup(ssid, pwd)
    wifi.setmode(3)
    ap_cfg={}
    ap_cfg.ssid=ssid
    if pwd ~= "" then 
        ap_cfg.pwd=pwd
    end
    ap_cfg.save=true
    wifi.ap.config(ap_cfg)
end

function ntp_sync()    
    if wifi.sta.getip() == nil then
        return false
    end
    sntp.sync(NTP_SERVER_IP,
        function(sec, usec, server, info)
            rtctime.set(sec+NTP_GMT*3600, usec)
            tm = rtctime.epoch2cal(rtctime.get())
            uart.write(1,string.format("%02d:%02d:%02d\n",tm["hour"], tm["min"], tm["sec"]))            
            return true
        end,
        function()
            --print('failed!')
            return false
        end
    )
end



function receiver(sck, data)
--Parse HTTP Request
  local response = {}
  local _, _, method, path, vars = string.find(data, "([A-Z]+) (.+)?(.+) HTTP")
  if method == nil then
    _, _, method, path = string.find(data, "([A-Z]+) (.+) HTTP")
  end
 --Parse GET Query String
  local _GET = {}
  if vars ~= nil then
    for id, value in string.gmatch(vars, "([%w_]+)=([%w._]+)&*") do
        _GET[id] = value
    end
  end

--Route
  if path == "/" then
    response[#response + 1] = "hello"    
  elseif path == "/wifi_sta_config" then
    response[#response + 1] = "<h1>Config Succeed!</h1>"
    response[#response + 1] = "<p>wifi_ssid: ".._GET["ssid"].."</P>"
    response[#response + 1] = "<p>wifi_password  :".._GET["password"].."</P>"
    WIFI_STA_SSID = _GET["ssid"]
    WIFI_STA_PW = _GET["password"]
    wifi_sta_setup(WIFI_STA_SSID, WIFI_STA_PW)
  elseif path == "/wifi_ap_config" then
    response[#response + 1] = "<h1>Config Succeed!</h1>"
    response[#response + 1] = "<p>wifi_ssid: ".._GET["ssid"].."</P>"
    response[#response + 1] = "<p>wifi_password  :".._GET["password"].."</P>"
    WIFI_AP_SSID = _GET["ssid"]
    WIFI_AP_PW = _GET["password"]
    wifi_ap_setup(WIFI_AP_SSID, WIFI_AP_PW)
  elseif path == "/ntp_config" then
    response[#response + 1] = "<h1>Config Succeed!</h1>"
    response[#response + 1] = "<p>server_ip : ".._GET["server_ip"].."</P>"
    response[#response + 1] = "<p>gmt :".._GET["gmt"].."</P>"
    response[#response + 1] = "<p>sync_feq :".._GET["sync_feq"].."</P>"
    NTP_SERVER_IP = _GET["server_ip"]
    NTP_SYNC_FEQ = _GET["update_feq"]
    NTP_GMT = _GET["gmt"]
  elseif path == "/get_ntp_config" then
    response[#response + 1] = "{"
    response[#response + 1] = "server_ip:\'"..NTP_SERVER_IP.."\',"
    response[#response + 1] = "gmt:\'"..NTP_GMT.."\',"
    response[#response + 1] = "sync_feq:"..NTP_SYNC_FEQ.."}"
  elseif path == "/get_wifi_sta_config" then
    response[#response + 1] = "{"
    response[#response + 1] = "ssid:\'"..WIFI_STA_SSID.."\',"
    response[#response + 1] = "rssi:"..wifi.sta.getrssi()..","
    response[#response + 1] = "ip:\'"..wifi.sta.getip().."\',"
    response[#response + 1] = "mac:\'"..wifi.sta.getmac().."\'}"
  elseif path == "/get_wifi_ap_config" then
    response[#response + 1] = "{"
    response[#response + 1] = "ssid:\'"..WIFI_AP_SSID.."\',"
    response[#response + 1] = "ip:\'"..wifi.ap.getip().."\',"
    response[#response + 1] = "mac:\'"..wifi.ap.getmac().."\'}"
  elseif path == "/ntp_sync" then
    ntp_sync()
    response[#response + 1] = "sync"
  else
    response[#response + 1] = "404 not found"
  end  

  -- Sends and removes the first element from the 'response' table
  local function send(localSocket)
    if #response > 0 then
      localSocket:send(table.remove(response, 1))
    else
      localSocket:close()
      response = nil
    end
  end
  -- Triggers the send() function again once the first chunk of data was sent
  sck:on("sent", send)
  send(sck)
end




                                                                                                          


