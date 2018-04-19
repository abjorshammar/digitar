import urllib2
from libgravatar import Gravatar
import PythonMagick as Magick
import binascii

# Set your variables here
# email = "christian.steinbach@digitalroute.com"
email = "goran.asp@digitalroute.com"
#email = "andreas.bjorshammar@digitalroute.com"
#email = "mattias.lundstrom@digitalroute.com"
#default = "https://www.example.com/default.jpg"

def xbm_to_base64(xbm):
  txt = []
  linenr=0
  for line in xbm.splitlines():
      line = line.strip()
      if linenr > 2:
          for part in line.split():
              part = part.strip(" ,")
              if part.startswith("0x"):
                part = part.replace("0x","")
                txt.append(part)
      linenr = linenr + 1
  hex_string = "".join(txt)
  binary_string = binascii.unhexlify(hex_string)
  return binascii.b2a_base64(binary_string)

def fetch_avatar(email):
  g = Gravatar(email)
  url=g.get_image(size=100)
  response = urllib2.urlopen(url)

  f = open("/tmp/tmp.png", "wb")
  f.write(response.read())
  f.close()

  img = Magick.Image("/tmp/tmp.png")
  img.magick('XBM')
  img.write("/tmp/out.xbm")
  f = open("/tmp/out.xbm", "rt")
  content = f.read()
  f.close()
  return xbm_to_base64(content)


if __name__ == "__main__":
  print(fetch_avatar(email))
