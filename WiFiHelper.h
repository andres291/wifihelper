

class WiFiHelper{
public:
  WiFiHelper(String hotsname);

  String Hostname;
  
  void tryToConnect();
  
  void firstConnect();
};