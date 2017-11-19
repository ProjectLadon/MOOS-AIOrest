# 1.0 MOOS Adafruit IO interface module {#SRD001 }

This is a connector between a MOOS community and Adafruit.io using the REST interface. The purpose of pAIOrest is to provide an easily configurable way to connect MOOS variables to Adafruit.io feeds.

## 1.1 Software Structure & Dependencies {#SRD002 }

### 1.1.1 SRD003 {#SRD003 }

This module **shall** be implemented as a MOOS Appcasting App as describe in the (http://oceanai.mit.edu/moos-ivp/pmwiki/pmwiki.php)[MOOS-IvP wiki].

### 1.1.2 SRD004 {#SRD004 }

This module **shall** use libcurl as the HTTP client.

### 1.1.3 SRD005 {#SRD005 }

This module **shall** use (http://www.rapidjson.org)[rapidjson] as the JSON parse/unparse library.

### 1.1.4 SRD006 {#SRD006 }

This module **shall** address the adafruit.io REST interface v2.0 or later

## 1.2 MOOS Startup Configuration Options {#SRD007 }

### 1.2.1 SRD008 {#SRD008 }

The configuration file **must** contain one and only one ```AIOconf``` entries.

#### 1.2.1.1 SRD009 {#SRD009 }

The value of this entry **must** be the name of a file containing a JSON document conforming to schema/configuration_schema.json.

This allows the sensitive username and API key to be stored separately from the rest of .moos file.

#### 1.2.1.2 SRD010 {#SRD010 }

The document **must** contain a ```url``` key pointing to the desired server, and the module **shall** make all connections to the specified server.

#### 1.2.1.3 SRD011 {#SRD011 }

The document **must** contain a ```username``` key. The module **shall** use this username to log in to the server.

#### 1.2.1.4 SRD012 {#SRD012 }

The document **must** contain a ```key``` key. The module **shall** use this API key when logging into the server.

### 1.2.2 SRD013 {#SRD013 }

The configuration file **may** contain one or more ```publish``` entries.

#### 1.2.2.1 SRD014 {#SRD014 }

The value of each entry **must** be a JSON document conforming to schema/publish_schema.json.

#### 1.2.2.2 SRD015 {#SRD015 }

The document **must** contain a ```variable``` key. The module **shall** subscribe to a community variable of that name.

#### 1.2.2.3 SRD016 {#SRD016 }

The document **must** contain a ```varType``` key. This **must** name the MOOS type of variable that the ```variable``` key points at. This module **shall** reject the variable if it is not of the named type.

#### 1.2.2.4 SRD017 {#SRD017 }

The document **must** contain a ```feed``` key. This module **shall** publish the values received on ```variable``` to this feed.

#### 1.2.2.5 SRD018 {#SRD018 }

The document **may** contain a ```group``` key. If present, this module **shall** publish to the named feed in the named group.

#### 1.2.2.6 SRD037 {#SRD037 }

The document **may** contain an ```updateOnly``` key. If it is true, then this module **shall** publish this item only when it is updated.

### 1.2.3 SRD019 {#SRD019 }

The configuration file **may** contain one or more ```subscribe``` entries.

#### 1.2.3.1 SRD020 {#SRD020 }

The value of each entry **must** be a JSON document conforming to schema/subscribe_schema.json.

#### 1.2.3.2 SRD021 {#SRD021 }

The document **must** contain a ```variable``` key. The module **shall** publish to a community variable of that name.

#### 1.2.3.3 SRD022 {#SRD022 }

The document **must** contain a ```varType``` key, which **must** name a MOOS type. This module **shall** publish the variable as the given type.

#### 1.2.3.4 SRD023 {#SRD023 }

The document **must** contain a ```feed``` key. This module **shall** publish the values received from this feed on ```variable```.

#### 1.2.3.5 SRD024 {#SRD024 }

The document **may** contain a ```group``` key. If present, this module **shall** subscribe to the named feed in the named group.

### 1.2.4 SRD025 {#SRD025 }

The configuration file **may** contain one ```subFile``` entries in addition to any ```subscribe``` entries. If present, it **must** contain a JSON array of ```subscribe``` objects. This module **shall** load the file and process its elements as if they were additional ```subscribe``` entries.

### 1.2.5 SRD026 {#SRD026 }

The configuration file **may** contain one ```pubFile``` entries in addition to any ```publish``` entries. If present, it **must** contain a JSON array of ```publish``` objects. This module **shall** load the file and process its elements as if they were additional ```publish``` entries.

### 1.2.6 SRD027 {#SRD027 }

The configuration file **may** contain one ```confFile``` entries in addition to any ```publish```, ```subscribe```, ```subFile```, and ```pubFile``` entries. If present, it **must** contain a JSON array of ```publish``` and ```subscribe``` objects. This module **shall** load the file and process its elements as if they were additional ```publish``` and ```subscribe``` entries.

### 1.2.7 SRD028 {#SRD028 }

If any of the file entries point to a file that is either missing or fails JSON validation, this module **shall** log an error and exit.

### 1.2.8 SRD029 {#SRD029 }

The configuration file **may** contain a ```pubFrequency``` entry.

#### 1.2.8.1 SRD030 {#SRD030 }

If present, this **shall** set the number of times per minute to publish outgoing data.

#### 1.2.8.2 SRD031 {#SRD031 }

If it is not present, it **shall** default to 60.

### 1.2.9 SRD032 {#SRD032 }

The configuration file **may** contain a ```subFrequency``` entry.

#### 1.2.9.1 SRD033 {#SRD033 }

If present, this **shall** set the number of times per minute to poll for incoming data.

#### 1.2.9.2 SRD034 {#SRD034 }

If it is not present, it **shall** default to ```pubFrequency```.

# 2.0 Outgoing Data {#SRD035 }

Adafruit.io limits incoming data updates to 60 data points per minute.

## 2.1 SRD038 {#SRD038 }

Every 60/pubFrequency seconds, this module **shall** transmit one published item to the server.

## 2.2 SRD039 {#SRD039 }

The items **shall** be transmitted in the order in which they were defined.

## 2.3 SRD040 {#SRD040 }

If an item has updateOnly set to true, then it **shall** be skipped unless the underlying MOOS variable has been updated since the last transmission.

# 3.0 Incoming Data {#SRD036 }

## 3.1 SRD041 {#SRD041 }

Every 60/subFrequency seconds, this module **shall** query all subscribed feeds.

## 3.2 SRD042 {#SRD042 }

Each subscribed feed **shall** be publishd to its corresponding MOOS variable if and only if a new data item is retrieved from the feed.

