#!/usr/bin/python

import httplib, json, base64, sys
#12401 Wyvern Board
#12303 Wyvern Bug Dashboard
SP="%20"
TEAM = '"team'+SP+'wyvern"'
URL_ROOT = '/rest/api/latest/search?jql='+\
           '"assigned'+SP+'team"='+TEAM\
           +SP+'and'+SP+'status!="All'+SP+'Fixes'+SP+'Closed"'\
           +SP+'and'+SP+'status!=closed'\
           +SP+'and'+SP+'status!=done'\
           +SP+'and'+SP+'status!="To'+SP+'Do"'\
           +SP+'and'+SP+'status!=new'\
           +SP+'and'+SP+'status!=open'\
           +SP+'and'+SP+'status!=candidate'\
           +SP+'and'+SP+'status!=verified'
           #+SP+'and'+SP+'type=bug'

USERNAME = "bugzillarestapi"
PASSWORD = '''-'!3cT}CAz3('''
headers = {'Authorization': 'Basic ' + base64.b64encode(USERNAME+':'+PASSWORD)}
headers.update({'Content-Type': 'application/json'})
headers.update({'Accept': '*/*'})

# Connect to Jira:
http = httplib.HTTPConnection('jira.digitalroute.com', 8080)
# And get the info on issues
http.request('GET', URL_ROOT, None, headers)
response = http.getresponse()
if response.status != 200:
    print "Uh-oh, response =", response.reason, response.status
    http.close()
    exit()
jsonResponse = response.read()
http.close()
myResponse = json.loads(jsonResponse)

for ii in range (0, len(myResponse['issues'])):
    print myResponse['issues'][ii]['key'],\
    myResponse['issues'][ii]['fields']['status']['name']#,\
    #myResponse['issues'][ii]['fields']['description']

# Create the JSON to be sent to AWS
awsShadow = {}
awsShadow['issues'] = [{}]
for jj in range (0, len(myResponse['issues'])):
    awsShadow['issues'].append({})
    awsShadow['issues'][jj]['key'] = myResponse['issues'][jj]['key']
    awsShadow['issues'][jj]['status'] = myResponse['issues'][jj]['fields']['status']['name']


print awsShadow

# Send it to the aws lambda API:
import urllib2
AWSLAMBDAAPI = 'https://5omnzjyeva.execute-api.eu-west-1.amazonaws.com/prod/digitar/hellopub'

read = urllib2.urlopen(AWSLAMBDAAPI) # Works for reading
print read
read.close

# Now write:
req  = urllib2.Request(AWSLAMBDAAPI, json.dumps(awsShadow), {'Content-Type': 'application/json'})
f = urllib2.urlopen(req)
response = f.read()
f.close()
