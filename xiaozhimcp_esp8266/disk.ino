// void listDir(const char *dirname) {
//   Serial.printf("Listing directory: %s\n", dirname);

//   Dir root = LittleFS.openDir(dirname);

//   while (root.next()) {
//     File file = root.openFile("r");
//     Serial.print("  FILE: ");
//     Serial.print(root.fileName());
//     Serial.print("  SIZE: ");
//     Serial.print(file.size());
//     time_t cr = file.getCreationTime();
//     time_t lw = file.getLastWrite();
//     file.close();
//     struct tm *tmstruct = localtime(&cr);
//     Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
//     tmstruct = localtime(&lw);
//     Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
//   }
// }
void saveMcpLink(String link) {
  File f = LittleFS.open("/mcp_link.txt", "w");
  if (!f) {
    Serial.println("❌ Failed to open file for writing");
    return;
  }
  f.print(link);
  f.close();
  Serial.println("✅ Saved MCP link to SPIFFS");
}

String loadMcpLink() {
  File f = LittleFS.open("/mcp_link.txt", "r");
  if (!f) {
    Serial.println("⚠️ File not found");
    return "";
  }
  String link = f.readString();
  f.close();
  Serial.println("📦 Loaded MCP link: " + link);
  return link;
}