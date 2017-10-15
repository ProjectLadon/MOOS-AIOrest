# MOOS-pAIOrest
This is a connector between a MOOS community and Adafruit.io using the REST interface. The purpose of pAIOrest is to provide an easily configurable way to connect MOOS variables to Adafruit.io feeds.

## Dependencies
* MOOS-IvP
* https://github.com/Tencent/rapidjson/ -- provides JSON parse/deparse
* https://curl.haxx.se/libcurl/ -- provides HTTP client interface

## Configuration Parameters
* AIOconf -- the path to a file containing JSON defining the overall connection. It must conform to the following schema:
```
{
	"$schema": "http://json-schema.org/schema#",
	"id": "AIO_conf",
	"type": "object",
	"properties":{
		"url": {
			"type":"string",
			"format":"uri"
		},
		"username":{"type":"string"},
		"key":{"type":"string"}
	},
	"required":["url","username","key"],
	"additionalProperties":false
}
```
Specifying the REST key in this manner permits storing the sensitive username and key information separately from the .moos file.
* publish -- a JSON object conforming to the following schema that describes a variable to publish. This may occur one or more times.
```
{
	"$schema": "http://json-schema.org/schema#",
	"id": "AIO_publish",
	"type": "object",
	"properties":{
		"direction":"publish",
		"updateOnly": {
			"type": "boolean",
			"$comment": "If this is true, the publish routine skips this variable if it has not changed since the last transmission."
		},
		"variable":{
			"type":"name", 
			"$comment":"This must be the name of a variable that exists in the MOOS community",
			"$comment":"It will be subscribed from MOOSDB and published to AIO on the given feed"
		},
		"varType":{
			"type":"string",
			"enum": ["DOUBLE", "STRING", "BINARY"],
			"$comment": "Must match the type of the named variable"
		},
		"group":{"type":"string"},
		"feed":{"type":"string"}
	},
	"required":["direction","variable","varType","feed"]
}
```
* pubFile -- a path to a file containing an array of ```publish``` objects. This can be used in conjunction with zero or more ```publish``` entries. 
* subscribe -- a JSON object conforming to the following schema that describes a variable to publish. This may occur one or more times. 
```
{
	"$schema": "http://json-schema.org/schema#",
	"id": "AIO_subscribe",
	"type": "object",
	"properties":{
		"direction":"subscribe",
		"variable":{
			"type":"name", 
			"$comment":"This must be the name of a variable that does not exist in the MOOS community",
			"$comment":"It will be published to MOOSDB from the given AIO feed"
		},
		"varType":{
			"type":"string",
			"enum": ["DOUBLE", "STRING", "BINARY"],
			"$comment": "Must match the type of the named variable"
		},
		"group":{"type":"string"},
		"feed":{"type":"string"}
	},
	"required":["direction","variable","varType","feed"]
}
```
* subFile -- a path to a file containing an array of ```subscribe``` objects. This can be used in conjunction with zero or more ```subscribe``` entries.
* confFile -- a path to a file containing an array of ```publish``` and/or ```subscribe``` objects. This can be used in conjunction with any other combination of publish and subscribe objects. 
* pubFrequency -- the number of times per minute to publish outgoing data. Defaults to 60.
* subFrequency -- the number of times per minute to poll for incoming data. Defaults to same value as ```pubFrequency```. 

## Incoming and Outgoing Variables
This is determined entirely by the publish and subscribe entries

## Outgoing Timing and Sequencing
Since Adafruit.io has hard limits on how often data can be written, pAIOrest must obey them in order to work correctly. It maintains a list of outgoing variables and steps through them one by one at the speed dictated by ```pubFrequency```. If a variable has not been updated since the last time it was transmitted, pAIOrest will skip it until the next time it runs through the list. 

## Incoming Notifications
A list of incoming variables is similarly maintained. pAIOrest polls for each one in turn at the frequency determined by ```subFrequency```.
