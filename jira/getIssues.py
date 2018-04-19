#!/usr/bin/python
# -*- coding: utf-8 -*- x
'''# -*- coding: ISO-8859-1 -*- x'''
import httplib, json, base64,sys
import urllib2
from fetchAvatar import fetch_avatar

if len(sys.argv) == 1:
    print "Please enter name of user"
    exit()
SP = "%20"
name = sys.argv[1].replace(" ", SP)
fixname = name.replace("Å","\xe4")
fixname = name.replace("å","\xe5")
fixname = name.replace("Ä","\xc4")
fixname = name.replace("ä","\xe4")
fixname = name.replace("Ö","\xd6")
fixname = name.replace("ö","\xf6")

URL_ROOT = '/rest/api/latest/search?jql='+\
           '"assignee"="'+fixname+'"'\
           +SP+'and'+SP+'status!="All'+SP+'Fixes'+SP+'Closed"'\
           +SP+'and'+SP+'status!=closed'\
           +SP+'and'+SP+'status!=done'\
           +SP+'and'+SP+'status!="To'+SP+'Do"'\
           +SP+'and'+SP+'status!=new'\
           +SP+'and'+SP+'status!=open'\
           +SP+'and'+SP+'status!=candidate'\
           +SP+'and'+SP+'status!=verified'
           #+SP+'and'+SP+'type=bug'
USERNAMEFILE = open('USERNAME', 'r')
USERNAME = USERNAMEFILE.read()
PASSWORDFILE = open('PASSWORD', 'r')
PASSWORD = PASSWORDFILE.read()
HEADERS = {'Authorization': 'Basic ' + base64.b64encode(USERNAME+':'+PASSWORD),\
           'CONTENT-TYPE': 'APPLICATION/JSON',\
           'Accept': '*/*'}

# Connect to Jira:
http = httplib.HTTPConnection('jira.digitalroute.com', 8080)
# And get the info on issues
http.request('GET', URL_ROOT, None, HEADERS)
response = http.getresponse()
if response.status != 200:
    print "Uh-oh, response =", response.reason, response.status
    http.close()
    exit()
jsonResponse = response.read()
http.close()
myResponse = json.loads(jsonResponse)
'''
for ii in range(0, len(myResponse['issues'])):
    print myResponse['issues'][ii]['key'],\
    myResponse['issues'][ii]['fields']['status']['name'],\
    myResponse['issues'][ii]['fields']['summary']
'''
# Create the JSON to be sent to AWS
awsShadow = {}
awsShadow['issues'] = [{}]
awsShadow['assignee'] = myResponse['issues'][0]['fields']['assignee']['displayName']
awsShadow['emailAddress'] = myResponse['issues'][0]['fields']['assignee']['emailAddress']
awsShadow['avatar'] = fetch_avatar(myResponse['issues'][0]['fields']['assignee']['emailAddress'])

for jj in range(0, len(myResponse['issues'])):
    awsShadow['issues'].append({})
    awsShadow['issues'][jj]['key'] = myResponse['issues'][jj]['key']
    awsShadow['issues'][jj]['status'] = myResponse['issues'][jj]['fields']['status']['name']
    awsShadow['issues'][jj]['summary'] = myResponse['issues'][jj]['fields']['summary']

print json.dumps(awsShadow)

# Send it to the aws lambda API:
AWSLAMBDAAPIPUT = 'https://5omnzjyeva.execute-api.eu-west-1.amazonaws.com/prod/digitar/put?key=test2'
req = urllib2.Request(AWSLAMBDAAPIPUT, json.dumps(awsShadow), {'Content-Type': 'application/json'})
f = urllib2.urlopen(req)
jsonresponse = f.read()
myresponse = json.loads(jsonresponse)
print myresponse['message']
f.close()

