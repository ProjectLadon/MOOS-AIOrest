# MOOS-AIOrest
This is a connector between a MOOS community and Adafruit.io using the REST interface. The purpose of this module is to provide an easily configurable way to connect MOOS variables to Adafruit.io feeds.

# Dependencies
* MOOS-IvP
* https://github.com/Tencent/rapidjson/ -- provides JSON parse/deparse
* https://curl.haxx.se/libcurl/ -- provides HTTP client interface

# Configuration Parameters
* AIOconf -- the path to a file containing JSON defining the overall connection. It must conform to the following schema:
```
{
	"$schema": "http://json-schema.org/schema#",
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
	"type": "object",
	"properties":{
		"variable":{
			"type":"name", 
			"$comment":"This must be the name of a variable that exists in the MOOS community"
			"$comment":"It will be subscribed from MOOSDB and published to AIO on the given feed"
		},
		"varType":{
			"type":"string","enum": ["DOUBLE", "STRING", "BINARY"],
			"$comment": "Must match the type of the named variable"
		},
		"group":{"type":"string"},
		"feed":{"type":"string"}
	},
	"required":["variable","varType","feed"]
}
```
* pubFile -- a path to a file containing an array of publish objects. This can be used in conjunction with zero or more publish entries. 
* subscribe -- a JSON object conforming to the following schema that describes a variable to publish. This may occur one or more times. 
```
{
	"$schema": "http://json-schema.org/schema#",
	"type": "object",
	"properties":{
		"variable":{
			"type":"name", 
			"$comment":"This must be the name of a variable that does not exist in the MOOS community"
			"$comment":"It will be published to MOOSDB from the given AIO feed"
		},
		"varType":{
			"type":"string","enum": ["DOUBLE", "STRING", "BINARY"],
			"$comment": "Must match the type of the named variable"
		},
		"group":{"type":"string"},
		"feed":{"type":"string"}
	},
	"required":["variable","varType","feed"]
}
```
* subFile -- a path to a file containing an array of subscribe objects. This can be used in conjunction with zero or more subscribe entries.
* pubFrequency -- the number of times per minute to publish outgoing data. 

# Incoming and Outgoing Variables
This is determined entirely by the publish and subscribe entries