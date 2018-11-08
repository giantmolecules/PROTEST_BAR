
from microWebSrv import MicroWebSrv
import network
import utime
# ----------------------------------------------------------------------------

@MicroWebSrv.route('/test')
def _httpHandlerTestGet(httpClient, httpResponse) :
	content = """\
	<!DOCTYPE html>
	<html lang=en>
        <head>
        	<meta charset="UTF-8" />
            <title>TEST GET</title>
        </head>
        <body>
            <h1>TEST GET</h1>
            Client IP address = %s
            <br />
			<form action="/test" method="post" accept-charset="ISO-8859-1">
			<textarea rows="4" cols="50" name="ssid">Enter text</textarea>
				<input type="submit" value="Submit">
			</form>
        </body>
    </html>
	""" % httpClient.GetIPAddr()
	httpResponse.WriteResponseOk( headers		 = None,
								  contentType	 = "text/html",
								  contentCharset = "UTF-8",
								  content 		 = content )


@MicroWebSrv.route('/test', 'POST')
def _httpHandlerTestPost(httpClient, httpResponse) :
	formData  = httpClient.ReadRequestPostedFormData()
	ssid = formData["ssid"]
	lines = ssid.split("\r")
	content   = """\
	<!DOCTYPE html>
	<html lang=en>
		<head>
			<meta charset="UTF-8" />
            <title>TEST POST</title>
        </head>
        <body>
            <h1>WiFi Config</h1>
            SSID = %s<br />
        </body>
    </html>
	""" % ( MicroWebSrv.HTMLEscape(ssid))
	httpResponse.WriteResponseOk( headers		 = None,
								  contentType	 = "text/html",
								  contentCharset = "UTF-8",
								  content 		 = content )

prefixes=["!","#","%","&","'","(",")","*","+",",","-",".","/"]
lines=["Erp", "Arp", "Urp"]

ap=network.WLAN(network.AP_IF)
ap.active(True)
ap.config(essid=">>>ESP<<<", authmode=0)
ap.ifconfig(('10.0.0.1','255.255.255.0','10.0.0.1','10.0.0.1'))

srv = MicroWebSrv(webPath='www/')
srv.SetNotFoundPageUrl("/test")

while(1):
	srv.Stop()
	for line in lines:
		ap.config(essid=line, authmode=0)
		utime.sleep(10)
	srv.Start(threaded=False)
	utime.sleep(30)
# ----------------------------------------------------------------------------
