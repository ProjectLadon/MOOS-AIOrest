# 1.0 Dependencies & Build configuration {#SDD001 }

## 1.1 SDD002 {#SDD002 }

This software **shall** be implemented as a MOOS Appcasting App as describe in the [MOOS-IvP wiki](http://oceanai.mit.edu/moos-ivp/pmwiki/pmwiki.php).

## 1.2 SDD003 {#SDD003 }

This software **shall** use [rapidjson](http://www.rapidjson.org) as the JSON parse/unparse library.

## 1.3 SDD004 {#SDD004 }

This software **shall** use libcurl as the HTTP client.

## 1.4 SDD005 {#SDD005 }

This software **shall** use [cppcoder](https://github.com/tplgy/cppcodec) for Base64 encoding/decoding.

## 1.5 SDD006 {#SDD006 }

This software **shall** be compiled using cmake.

## 1.6 SDD007 {#SDD007 }

Unit tests **shall** be written using the gtest suite.

## 1.7 SDD008 {#SDD008 }

The cmake list **shall** generate a MOOS executable target.

## 1.8 SDD009 {#SDD009 }

The cmake list **shall** generate a gtest target called ``unit_tests``.

## 1.9 SDD010 {#SDD010 }

The cmake list **shall** generate a target that compiles and runs the tests called ``test``.

# 2.0 Data Types {#SDD011 }

## 2.1 ``AIOconf`` class {#SDD012 }

### 2.1.1 Constructor {#SDD013 }

#### 2.1.1.1 SDD014 {#SDD014 }

``AIOconf(rapidjson::Document &d)`` **shall** validate ``d`` with the ``AIO_conf`` schema and populate the members of this object from it. If d does not pass validation, this method will force the program to exit immediately.

### 2.1.2 Public Methods {#SDD015 }

#### 2.1.2.1 ``std::unique_ptr<rapidjson::Document> get(std::string feed, std::string group)`` {#SDD016 }

##### 2.1.2.1.1 SDD017 {#SDD017 }

This method **shall** return the last data item in the given feed and group.

##### 2.1.2.1.2 SDD018 {#SDD018 }

This method **shall** parse and validate the returned JSON using the ``AIO_data_response`` schema.

##### 2.1.2.1.3 SDD019 {#SDD019 }

If it does not parse or fails validation, this method **shall** return ``nullptr``.

#### 2.1.2.2 ``std::unique_ptr<rapidjson::Document> put(const std::string feed, const td::string group, const double data, int *httpcode = nullptr)`` {#SDD020 }

##### 2.1.2.2.1 SDD021 {#SDD021 }

This method **shall** stringify and upload the value in ``data`` to the given ``feed`` and ``group``.

##### 2.1.2.2.2 SDD022 {#SDD022 }

This data **shall** be tagged with the provided ``lat`` and ``lon``.

##### 2.1.2.2.3 SDD023 {#SDD023 }

The https return code **shall** be stored in ``httpcode``, if present.

##### 2.1.2.2.4 SDD024 {#SDD024 }

Returned JSON **shall** be parsed and returned. If it does not parse, this method **shall** return ``nullptr``.

#### 2.1.2.3 ``std::unique_ptr<rapidjson::Document> put(const std::string feed, const std::string group, const std::string data, int *httpcode = nullptr)`` {#SDD025 }

##### 2.1.2.3.1 SDD026 {#SDD026 }

This method **shall** upload the value in ``data`` to the given ``feed`` and ``group``.

##### 2.1.2.3.2 SDD027 {#SDD027 }

This data **shall** be tagged with the provided ``lat`` and ``lon``.

##### 2.1.2.3.3 SDD028 {#SDD028 }

The https return code **shall** be stored in ``httpcode``, if present.

##### 2.1.2.3.4 SDD029 {#SDD029 }

Returned JSON **shall** be parsed and returned. If it does not parse, this method **shall** return ``nullptr``.

#### 2.1.2.4 ``bool publish()`` {#SDD030 }

##### 2.1.2.4.1 SDD031 {#SDD031 }

This method **shall** call the ``isUpdateOnly()`` method of the element of ``publishers`` that the ``publishPtr`` currently points to. If it is ``true``, this method **shall** check if the element ``isFresh()``. If this is false, this method **shall** iterate through the ``publishers`` vector, wrapping around once, until it either finds an element that passes these tests or reaches its starting point. If it reaches the starting point, this method **shall** return ``false``.

##### 2.1.2.4.2 SDD032 {#SDD032 }

This method **shall** call the ``publish()`` method of the members of ``publishers`` that ``publishPtr`` currently points to.

##### 2.1.2.4.3 SDD033 {#SDD033 }

This method **shall** increment ``publishPtr`` with wrap.

##### 2.1.2.4.4 SDD034 {#SDD034 }

This method **shall** return the value returned by the ``publish()`` call.

#### 2.1.2.5 ``bool poll(AIOrest *a)`` {#SDD035 }

##### 2.1.2.5.1 SDD036 {#SDD036 }

This method **shall** call the ``poll()`` method of each member of ``subscribers`` in turn.

##### 2.1.2.5.2 SDD037 {#SDD037 }

If any of these calls return ``true``, this method **shall** return true. Otherwise, it **shall** return false.

#### 2.1.2.6 ``bool procMail(CMOOSMsg &msg)`` {#SDD038 }

##### 2.1.2.6.1 SDD039 {#SDD039 }

This method **shall** call the ``procMail()`` method of each member of the ``publishers()`` list.

##### 2.1.2.6.2 SDD040 {#SDD040 }

If ``msg.GetKey() == latVar``, this method **shall** store the value in the ``lat`` member.

##### 2.1.2.6.3 SDD041 {#SDD041 }

If ``msg.GetKey() == lonVar``, this method **shall** store the value in the ``lon`` member.

##### 2.1.2.6.4 SDD042 {#SDD042 }

If any of these calls returns ``true``, this method **shall** return ``true``. Otherwise, it **shall** return ``false``.

#### 2.1.2.7 SDD043 {#SDD043 }

``std::vector<unique_ptr<Publisher>> &getPublishers()`` **shall** return a reference to the ``publishers`` member.

#### 2.1.2.8 SDD044 {#SDD044 }

``std::vector<unique_ptr<Subscriber>> &getSubscribers()`` **shall** return a reference to the ``subscribers`` member.

#### 2.1.2.9 SDD045 {#SDD045 }

``bool addPublisher(rapidjson::Value &v)`` **shall** construct a ``Publisher`` method from the provided JSON and, if construction is successful, add it to ``publishers`` and return true.

#### 2.1.2.10 SDD046 {#SDD046 }

``bool addSubscriber(rapidjson::Value &v)`` **shall** construct a ``Subscriber`` method from the provided JSON and, if construction is successful, add it to ``subscribers`` and return true.

#### 2.1.2.11 SDD047 {#SDD047 }

``const std::string &getURL()`` **shall** return a reference to the url member.

### 2.1.3 Private Methods {#SDD048 }

#### 2.1.3.1 ``std::unique_ptr<rapidjson::Document> makeRequest (std::string url, rapidjson::Document *request)`` {#SDD049 }

##### 2.1.3.1.1 SDD050 {#SDD050 }

This method **shall** assemble and execute an HTTP request against the given URL.

##### 2.1.3.1.2 SDD051 {#SDD051 }

If ``request == nullptr``, it *shall** execute a GET request; otherwise, it **shall** execute a POST request with the given JSON as the body.

##### 2.1.3.1.3 SDD052 {#SDD052 }

This method **shall** parse the response and return a pointer to the resulting JSON document.

##### 2.1.3.1.4 SDD053 {#SDD053 }

If the response is not valid JSON, it **shall** return ``nullptr``.

### 2.1.4 Private Members {#SDD054 }

#### 2.1.4.1 SDD055 {#SDD055 }

``std::string url`` **shall*** store the URL of the Adafruit I/O REST server

#### 2.1.4.2 SDD056 {#SDD056 }

``std::string username`` **shall*** store the Adafruit I/O username

#### 2.1.4.3 SDD057 {#SDD057 }

``std::string key`` **shall** store the Adafruit I/O API key

#### 2.1.4.4 SDD058 {#SDD058 }

``double lat`` **shall** store the latitude

#### 2.1.4.5 SDD059 {#SDD059 }

``double lon`` **shall** store the longitude

#### 2.1.4.6 SDD060 {#SDD060 }

``std::string latVar`` **shall** store the latitude variable

#### 2.1.4.7 SDD061 {#SDD061 }

``std::string lonVar`` **shall** store the longitude variable

#### 2.1.4.8 SDD062 {#SDD062 }

``std::vector<std::unique_ptr<Publisher>> publishers`` stores the list of objects to publish to the AIO server

#### 2.1.4.9 SDD063 {#SDD063 }

``std::vector<std::unique_ptr<Subscriber>> subscribers`` stores the list of objects to subscribe to from the AIO server

#### 2.1.4.10 SDD064 {#SDD064 }

``std::iterator<std::vector<std::unique_ptr<Publisher>>> publishPtr`` is an iterator that points to the publish item that will be transmitted on the next call to ``publish()``.

## 2.2 ``Publisher`` virtual base class {#SDD065 }

### 2.2.1 SDD066 {#SDD066 }

The constructor for the base class **shall** be default

### 2.2.2 Static Public Methods {#SDD067 }

#### 2.2.2.1 ``std::unique_ptr<Publisher> publisherFactory(AIOconf *iface, rapidjson::Value &v)`` {#SDD068 }

##### 2.2.2.1.1 SDD069 {#SDD069 }

This method shall validate ``v`` against the ``AIO_publish`` schema, and return ``nullptr`` if it does not conform.

##### 2.2.2.1.2 SDD070 {#SDD070 }

This method **shall** select a subclass to construct based on the value of ``varType``.

##### 2.2.2.1.3 SDD071 {#SDD071 }

``DOUBLE`` **shall** result in the construction of a ``PublisherDouble``, ``STRING`` **shall** result in construction of a ``PublisherString``, and ``BINARY`` **shall** result in construction of a ``PublisherBinary`` object.

##### 2.2.2.1.4 SDD072 {#SDD072 }

This method **shall** pass its arguments unaltered to the chosen constructor.

##### 2.2.2.1.5 SDD073 {#SDD073 }

This method **shall** return a ``std::unique_ptr`` containing the constructed object.

### 2.2.3 Public Methods {#SDD074 }

#### 2.2.3.1 SDD075 {#SDD075 }

``virtual bool publish()`` **shall** publish the stored data to Adafruit IO and returns ``true`` if successful.

#### 2.2.3.2 SDD076 {#SDD076 }

``virtual bool procMail(CMOOSMsg &msg)`` **shall** compare the key of ``msg`` to the subscribed variable. If found, it **shall** store the data and return ``true``. Otherwise, it **shall** return ``false``.

#### 2.2.3.3 SDD077 {#SDD077 }

``virtual ACTable buildReport()`` **shall** produce a table for the AppCast report.

#### 2.2.3.4 SDD078 {#SDD078 }

``bool subscribe()`` **shall** subscribe to ``varName``.

#### 2.2.3.5 SDD079 {#SDD079 }

``bool isFresh()`` **shall** return the value of ``fresh``.

#### 2.2.3.6 SDD080 {#SDD080 }

``bool isUpdateOnly()`` **shall** return the value of ``updateOnly``

### 2.2.4 Protected Methods {#SDD081 }

#### 2.2.4.1 SDD082 {#SDD082 }

``void setFresh(bool n)`` sets ``fresh`` equal to ``n``

### 2.2.5 Protected Members {#SDD083 }

#### 2.2.5.1 SDD084 {#SDD084 }

``bool fresh`` **shall** be true if new data has been received from the MOOSDB but not yet published to Adafruit IO.

#### 2.2.5.2 SDD085 {#SDD085 }

``bool updateOnly`` **shall** be true if this item is intended to publish only on update.

#### 2.2.5.3 SDD086 {#SDD086 }

``std::string varName`` **shall** store the MOOSDB variable name.

#### 2.2.5.4 SDD087 {#SDD087 }

``std::string feed`` **shall** store the name of the Adafruit IO feed to publish to

#### 2.2.5.5 SDD088 {#SDD088 }

``std::string group`` **shall** store the name of the group which contains the target feed.

#### 2.2.5.6 SDD089 {#SDD089 }

``AIOconf *iface`` **shall** store a pointer to the interface.

## 2.3 ``PublisherString`` class {#SDD090 }

### 2.3.1 Constructor {#SDD091 }

#### 2.3.1.1 ``PublisherString(AIOconf *iface, rapidjson::Value &v)`` {#SDD092 }

##### 2.3.1.1.1 SDD093 {#SDD093 }

This constructor **shall** set the ``iface`` member to the incoming ``iface`` value.

##### 2.3.1.1.2 SDD094 {#SDD094 }

The ``varName`` member **shall** be set to the value of ``variable``.

##### 2.3.1.1.3 SDD095 {#SDD095 }

THe ``feed``, ``group``, and ``updateOnly`` members shall be set to the values of the corresponding JSON keys.

### 2.3.2 Public Methods {#SDD096 }

#### 2.3.2.1 SDD097 {#SDD097 }

``bool publish()`` **shall** call ``iface->put()`` with the value of ``data`` and return ``true`` if successful. Success **shall** be defined as an HTTP response code equal to 200 and a valid JSON response.

#### 2.3.2.2 SDD098 {#SDD098 }

``bool procMail(CMOOSMsg &msg)`` checks if ``msg.GetKey() == varName``. If it does, this method **shall** set ``data = msg.GetString()`` and return ``true``. Otherwise, it **shall** return ``false``.

#### 2.3.2.3 SDD099 {#SDD099 }

``ACTable buildReport()`` **shall** return a one-column table with ``varName`` as the header and ``data`` as the entry.

### 2.3.3 Private Members {#SDD100 }

#### 2.3.3.1 SDD101 {#SDD101 }

``std::string data`` **shall** store the data to publish.

## 2.4 ``PublisherDouble`` class {#SDD102 }

### 2.4.1 Constructor {#SDD103 }

#### 2.4.1.1 ``PublisherDouble(AIOconf *iface, rapidjson::Value &v)`` {#SDD104 }

##### 2.4.1.1.1 SDD105 {#SDD105 }

This constructor **shall** set the ``iface`` member to the incoming ``iface`` value.

##### 2.4.1.1.2 SDD106 {#SDD106 }

The ``varName`` member **shall** be set to the value of ``variable``.

##### 2.4.1.1.3 SDD107 {#SDD107 }

THe ``feed``, ``group``, and ``updateOnly`` members shall be set to the values of the corresponding JSON keys.

### 2.4.2 Public Methods {#SDD108 }

#### 2.4.2.1 SDD109 {#SDD109 }

``bool publish()`` **shall** call ``iface->put()`` with the value of ``data`` and return ``true`` if successful. Success **shall** be defined as an HTTP response code equal to 200 and a valid JSON response.

#### 2.4.2.2 SDD110 {#SDD110 }

``bool procMail(CMOOSMsg &msg)`` checks if ``msg.GetKey() == varName``. If it does, this method **shall** set ``data = msg.GetDouble()`` and return ``true``. Otherwise, it **shall** return ``false``.

#### 2.4.2.3 SDD111 {#SDD111 }

``ACTable buildReport()``**shall** return a one-column table with ``varName`` as the header and ``data`` as the entry.

### 2.4.3 Private Members {#SDD112 }

#### 2.4.3.1 SDD113 {#SDD113 }

``double data`` **shall** store the data to publish.

## 2.5 ``PublisherBinary`` class {#SDD114 }

### 2.5.1 Constructor {#SDD115 }

#### 2.5.1.1 ``PublisherBinary(AIOconf *iface, rapidjson::Value &v)`` {#SDD116 }

##### 2.5.1.1.1 SDD117 {#SDD117 }

This constructor **shall** set the ``iface`` member to the incoming ``iface`` value.

##### 2.5.1.1.2 SDD118 {#SDD118 }

The ``varName`` member **shall** be set to the value of ``variable``.

##### 2.5.1.1.3 SDD119 {#SDD119 }

THe ``feed``, ``group``, and ``updateOnly`` members shall be set to the values of the corresponding JSON keys.

### 2.5.2 Public Methods {#SDD120 }

#### 2.5.2.1 SDD121 {#SDD121 }

``bool publish()``**shall** call ``iface->put()`` with the string returned by ``encode()`` and return ``true`` if successful. Success **shall** be defined as an HTTP response code equal to 200 and a valid JSON response.

#### 2.5.2.2 SDD122 {#SDD122 }

``bool procMail(CMOOSMsg &msg)`` checks if ``msg.GetKey() == varName``. If it does, this method **shall** store the data in the message and return ``true``. Otherwise, it **shall** return ``false``.

#### 2.5.2.3 SDD123 {#SDD123 }

``ACTable buildReport()`` **shall** return a one-column table with ``varName`` as the header and the outputs of ``encode()`` as the entry.

### 2.5.3 Private Methods {#SDD124 }

#### 2.5.3.1 SDD125 {#SDD125 }

``std::string encode()`` **shall** return ``data`` encoded as URL-safe Base64 per RFC 4648.

### 2.5.4 Private Members {#SDD126 }

#### 2.5.4.1 SDD127 {#SDD127 }

``std::vector<unsigned char> data`` **shall** store the data to be published.

## 2.6 ``Subscriber`` virtual base class {#SDD128 }

### 2.6.1 SDD129 {#SDD129 }

The constructor for the base class **shall** be default

### 2.6.2 Static Public Methods {#SDD130 }

#### 2.6.2.1 ``std::unique_ptr<Subscriber> subscriberFactory(AIOconf *iface, rapidjson::Value &v)`` {#SDD131 }

##### 2.6.2.1.1 SDD132 {#SDD132 }

This method shall validate ``v`` against the ``AIO_subscribe`` schema, and return ``nullptr`` if it does not conform.

##### 2.6.2.1.2 SDD133 {#SDD133 }

This method **shall** select a subclass to construct based on the value of ``varType``.

##### 2.6.2.1.3 SDD134 {#SDD134 }

``DOUBLE`` **shall** result in the construction of a ``SubscriberDouble``, ``STRING`` **shall** result in construction of a ``SubscriberString``, and ``BINARY`` **shall** result in construction of a ``SubscriberBinary`` object.

##### 2.6.2.1.4 SDD135 {#SDD135 }

This method **shall** pass its arguments unaltered to the chosen constructor.

##### 2.6.2.1.5 SDD136 {#SDD136 }

This method **shall** return a ``std::unique_ptr`` containing the constructed object.

### 2.6.3 Public Methods {#SDD137 }

#### 2.6.3.1 SDD138 {#SDD138 }

``virtual bool poll(AIOrest *a)`` **shall** poll Adafruit IO for updates to the subscribed feed and return ``true`` if it is successful and new data is present.

#### 2.6.3.2 SDD139 {#SDD139 }

``virtual ACTable buildReport()`` **shall** produce a table for the AppCast report.

### 2.6.4 Protected Methods {#SDD140 }

#### 2.6.4.1 SDD141 {#SDD141 }

``bool isNew(std::string newTime)`` **shall** compare the the value in ``lastTime`` to ``newTime`` and return ``true`` iff they are not equal.

#### 2.6.4.2 SDD142 {#SDD142 }

``bool isNew(long int epoch)`` **shall** compare the the value in ``lastEpoch`` to the given ``epoch`` and return ``true`` iff ``epoch > lastEpoch``.

### 2.6.5 Protected Members {#SDD143 }

#### 2.6.5.1 SDD144 {#SDD144 }

``std::string varName`` **shall** store the MOOSDB variable name.

#### 2.6.5.2 SDD145 {#SDD145 }

``std::string feed`` **shall** store the name of the Adafruit IO feed to subscribe to

#### 2.6.5.3 SDD146 {#SDD146 }

``std::string group`` **shall** store the name of the group which contains the target feed.

#### 2.6.5.4 SDD147 {#SDD147 }

``std::string lastTime`` **shall** store the value of the ``created_at``item in the last data item returned

#### 2.6.5.5 SDD148 {#SDD148 }

``long int lastEpoch`` **shall** store the value of the ``created_epoch`` item in the last data item returned

#### 2.6.5.6 SDD149 {#SDD149 }

``AIOconf *iface`` **shall** store a pointer to the interface.

## 2.7 ``SubscriberString`` class {#SDD150 }

### 2.7.1 Constructor {#SDD151 }

#### 2.7.1.1 ``SubscriberString(AIOconf *iface, rapidjson::Value &v)`` {#SDD152 }

##### 2.7.1.1.1 SDD153 {#SDD153 }

This constructor **shall** set the ``iface`` member to the incoming ``iface`` value.

##### 2.7.1.1.2 SDD154 {#SDD154 }

The ``varName`` member **shall** be set to the value of ``variable``.

##### 2.7.1.1.3 SDD155 {#SDD155 }

THe ``feed``, ``group``, and ``updateOnly`` members shall be set to the values of the corresponding JSON keys.

### 2.7.2 Public Methods {#SDD156 }

#### 2.7.2.1 SDD157 {#SDD157 }

``bool poll(AIOrest *a)`` **shall** call ``iface->get()`` against ``feed`` and ``group``. Iff successful, it **shall** write the contents of the ``value`` key to ``data`` and return ``true``.

#### 2.7.2.2 SDD158 {#SDD158 }

``ACTable buildReport()`` **shall** return a two-column table with ``varName`` and "Last Time" as the headers and ``data`` and ``lastTime`` as the entries.

### 2.7.3 Private Members {#SDD159 }

#### 2.7.3.1 SDD160 {#SDD160 }

``std::string data`` shall store the data received

## 2.8 ``SubscriberDouble`` class {#SDD161 }

### 2.8.1 Constructor {#SDD162 }

#### 2.8.1.1 ``SubscriberDouble(AIOconf *iface, rapidjson::Value &v)`` {#SDD163 }

##### 2.8.1.1.1 SDD164 {#SDD164 }

This constructor **shall** set the ``iface`` member to the incoming ``iface`` value.

##### 2.8.1.1.2 SDD165 {#SDD165 }

The ``varName`` member **shall** be set to the value of ``variable``.

##### 2.8.1.1.3 SDD166 {#SDD166 }

THe ``feed``, ``group``, and ``updateOnly`` members shall be set to the values of the corresponding JSON keys.

### 2.8.2 Public Methods {#SDD167 }

#### 2.8.2.1 SDD168 {#SDD168 }

``bool tick(AIOrest *a)`` **shall** call ``iface->get()`` against ``feed`` and ``group``. Iff successful, it **shall** parse the contents of the ``value`` key into a double, store it in ``data``, and return ``true``.

#### 2.8.2.2 SDD169 {#SDD169 }

``ACTable buildReport()`` **shall** return a two-column table with ``varName`` and "Last Time" as the headers and ``data`` and ``lastTime`` as the entries.

### 2.8.3 Private Members {#SDD170 }

#### 2.8.3.1 SDD171 {#SDD171 }

``double data`` **shall** store the last data received.

## 2.9 ``SubscriberBinary`` class {#SDD172 }

### 2.9.1 Constructor {#SDD173 }

#### 2.9.1.1 ``SubscriberBinary(AIOconf *iface, rapidjson::Value &v)`` {#SDD174 }

##### 2.9.1.1.1 SDD175 {#SDD175 }

This constructor **shall** set the ``iface`` member to the incoming ``iface`` value.

##### 2.9.1.1.2 SDD176 {#SDD176 }

The ``varName`` member **shall** be set to the value of ``variable``.

##### 2.9.1.1.3 SDD177 {#SDD177 }

THe ``feed``, ``group``, and ``updateOnly`` members shall be set to the values of the corresponding JSON keys.

### 2.9.2 Public Methods {#SDD178 }

#### 2.9.2.1 SDD179 {#SDD179 }

``bool tick(AIOrest *a)`` **shall** call ``iface->get()`` against ``feed`` and ``group``. Iff successful, it **shall** pass the contents of the ``value`` key to ``decode()`` and return the result.

#### 2.9.2.2 SDD180 {#SDD180 }

``ACTable buildReport()`` **shall** return a two-column table with ``varName`` and "Last Time" as the headers and a hex dump of ``data`` and ``lastTime`` as the entries.

### 2.9.3 Private Methods {#SDD181 }

#### 2.9.3.1 SDD182 {#SDD182 }

``bool decode(std::string base64)`` **shall** decode the given string as URL-safe Base64 per RFC 4648 and store it in ``data``. It **shall** return ``true`` iff this operation succeeds.

### 2.9.4 Private Members {#SDD183 }

#### 2.9.4.1 SDD184 {#SDD184 }

``std::vector<unsigned char> data`` **shall** store the last data received.

# 3.0 ``AIOrest`` Private Members {#SDD185 }

## 3.1 SDD186 {#SDD186 }

``AIOconf conf`` **shall** store the interface configuration and the vectors of publish and subscribe objects.

## 3.2 SDD187 {#SDD187 }

``int pubFrequency`` **shall** store the number of times per minute to publish outgoing data. It **shall** default to 60.

## 3.3 SDD188 {#SDD188 }

``int subFrequency`` **shall** store the number of times per minutes to poll for incoming data. It **shall** default to the value of ``pubFrequency``.

## 3.4 SDD200 {#SDD200 }

``std::chrono::system_time lastPub`` **shall** store the time of the last successful publication.

## 3.5 SDD201 {#SDD201 }

``std::chrono::system_time lastSub`` **shall** store the time of the last successful subscription poll.

# 4.0 Startup Behavior {#SDD189 }

## 4.1 SDD202 {#SDD202 }

The software **shall** scan the startup block for an ``AIOconf`` key, parse the value, and construct an ``AIOconf`` object with it.

## 4.2 SDD191 {#SDD191 }

The software **shall** exit with an error if an ``AIOconf`` key is not present in the startup block, or if it is unable to successfully parse the attached JSON.

## 4.3 SDD190 {#SDD190 }

The software **shall** iterate through the startup block looking for the following keys:
    * publish
    * pubFile
    * subscribe
    * subFile
    * confFile
    * subFrequency
    * pubFrequency

## 4.4 SDD203 {#SDD203 }

The software **shall** exit with an error if the configuration block does not contain at least one of ``publish``, ``subscribe``, ``pubFile``, ``subFile``, or ``confFile``.

## 4.5 SDD204 {#SDD204 }

On any ``publish`` key, the software **shall** parse the value as JSON (exiting if parsing fails). If it successfully parses, it shall pass the resulting ``rapidjson::Document`` object to ``conf->addPublisher()``. If it returns ``false``, the software **shall** exit with an error.

## 4.6 SDD205 {#SDD205 }

On any ``subscribe`` key, the software **shall** parse the value as JSON (exiting if parsing fails). If it successfully parses, it shall pass the resulting ``rapidjson::Document`` object to ``conf->addSubscriber()``. If it returns ``false``, the software **shall** exit with an error.

## 4.7 SDD206 {#SDD206 }

On any ``pubFile`` key, the software **shall** attempt to load the named file and parse it as JSON (exiting if parsing fails). If it successfully parses and contains a JSON array, it **shall** pass each element of that array to ``conf->addPublisher()``. If  it does not contain an array or any return ``false``, the software **shall** exit with an error.

## 4.8 SDD207 {#SDD207 }

On any ``subFile`` key, the software **shall** attempt to load the named file and parse it as JSON (exiting if parsing fails). If it successfully parses and contains a JSON array, it **shall** pass each element of that array to ``conf->addSubscriber()``. If it does not contain an array or any return ``false``, the software **shall** exit with an error.

## 4.9 SDD208 {#SDD208 }

On any ``confFile`` key, the software **shall** attempt to load the named file and parse it as JSON (exiting if parsing fails). If it successfully parses and contains a JSON array, each element **shall** be passed to ``conf->addPublisher()`` and ``conf->addSubscriber()`` in turn. If the file does not contain a JSON array or both of these calls return ``false`` for any element, the softare **shall** exit with an error.

## 4.10 SDD209 {#SDD209 }

On a ``pubFrequency`` key, the software **shall** attempt to extract an integer from the value and store it in ``pubFrequency``.

## 4.11 SDD210 {#SDD210 }

On a ``subFrequency`` key, the software **shall** attempt to extract an integer from the value and store it in ``subFrequency``.

# 5.0 MOOS Community Connection Behavior {#SDD193 }

## 5.1 SDD194 {#SDD194 }

The ``OnConnectToServer()`` method **shall** iterate through the members of ``publishers`` and call the ``subscribe()`` method of each one.

# 6.0 New Mail Behavior {#SDD195 }

## 6.1 SDD196 {#SDD196 }

The ``OnNewMail()`` method **shall** iterate through the members of ``publishers`` and call the ``procMail()`` method of each one against each incoming message.

## 6.2 SDD197 {#SDD197 }

If none of the ``procMail()`` methods return true and the message is not an appcast message, the software **shall** log an error.

# 7.0 SDD198 {#SDD198 }

App Tick Behavior

## 7.1 SDD211 {#SDD211 }

On each tick, the software **shall** check whether ``60/subFrequency`` seconds have passed since the time recorded in ``lastSub``. If so, it **shall** call ``conf->poll(this)`` and set ``lastSub`` to the current time if it returns ``true``.

## 7.2 SDD212 {#SDD212 }

On each tick, the software **shall** check whether ``60/pubFrequency`` seconds have passed since the time recorded in ``lastPub``. If so, it **shall** call ``conf->publish()`` and set ``lastPub`` to the current time if it returns ``true``.

# 8.0 SDD199 {#SDD199 }

Appcast Reporting Behavior

## 8.1 SDD213 {#SDD213 }

This method **shall** print a "Publisher" header, and follow it with the results of the ``buildReport()`` method of each member of ``conf->getPublishers()``.

## 8.2 SDD214 {#SDD214 }

This method **shall** print a "Subscriber" header, and follow it with the results of the ``buildReport()`` method of each member of ``conf->getSubscribers()``.

