#include "core/stdafx.h"

#include "PUPPlaylist.h"

#include <filesystem>

/*
   playlists.pup: ScreenNum,Folder,Des,AlphaSort,RestSeconds,Volume,Priority
   PuP Pack Editor: Folder (Playlist),Description,Randomize,RestSeconds,Volume,Priority

   mappings:

     ScreenNum = ?
     Folder = Folder (Playlist)
     Des = Description
     AlphaSort = Randomize
     RestSeconds = RestSeconds
     Volume = Volume
     Priority = Priority

   notes:

     AlphaSort=0 is Randomize checked
*/

const char* PUP_PLAYLIST_FUNCTION_STRINGS[] = {
   "PUP_PLAYLIST_FUNCTION_DEFAULT",
   "PUP_PLAYLIST_FUNCTION_OVERLAYS",
   "PUP_PLAYLIST_FUNCTION_FRAMES",
   "PUP_PLAYLIST_FUNCTION_ALPHAS",
   "PUP_PLAYLIST_FUNCTION_SHAPES"
};

const char* PUP_PLAYLIST_FUNCTION_TO_STRING(PUP_PLAYLIST_FUNCTION value)
{
   if ((int)value < 0 || (size_t)value >= sizeof(PUP_PLAYLIST_FUNCTION_STRINGS) / sizeof(PUP_PLAYLIST_FUNCTION_STRINGS[0]))
      return "UNKNOWN";
   return PUP_PLAYLIST_FUNCTION_STRINGS[value];
}

PUPPlaylist::PUPPlaylist(const string& szFolder, const string& szDescription, bool randomize, int restSeconds, float volume, int priority)
{
   m_szFolder = szFolder;
   m_szDescription = szDescription;
   m_randomize = randomize;
   m_restSeconds = restSeconds;
   m_volume = volume;
   m_priority = priority;
   m_lastIndex = 0;

   if (StrCompareNoCase(szFolder, "PUPOverlays"))
      m_function = PUP_PLAYLIST_FUNCTION_OVERLAYS;
   else if (StrCompareNoCase(szFolder, "PUPFrames"))
      m_function = PUP_PLAYLIST_FUNCTION_FRAMES;
   else if (StrCompareNoCase(szFolder, "PUPAlphas"))
      m_function = PUP_PLAYLIST_FUNCTION_ALPHAS;
   else if (StrCompareNoCase(szFolder, "PuPShapes"))
      m_function = PUP_PLAYLIST_FUNCTION_SHAPES;
   else
      m_function = PUP_PLAYLIST_FUNCTION_DEFAULT;

   m_szBasePath = find_case_insensitive_directory_path(PUPManager::GetInstance()->GetPath() + szFolder);
   if (m_szBasePath.empty()) {
      PLOGE.printf("Playlist folder not found: %s", szFolder.c_str());
      return;
   }

   for (const auto& entry : std::filesystem::directory_iterator(m_szBasePath)) {
      if (entry.is_regular_file()) {
         string szFilename = entry.path().filename();
         if (!szFilename.empty() && szFilename[0] != '.') {
            m_files.push_back(szFilename);
            m_fileMap[lowerCase(szFilename)] = szFilename;
         }
      }
   }
   std::sort(m_files.begin(), m_files.end());
}

PUPPlaylist::~PUPPlaylist()
{
   m_files.clear();
}

PUPPlaylist* PUPPlaylist::CreateFromCSV(const string& line)
{
   vector<string> parts = parse_csv_line(line);
   if (parts.size() != 7) {
      PLOGW.printf("Invalid playlist: %s", line.c_str());
      return nullptr;
   }

   string szFolderPath = find_case_insensitive_directory_path(PUPManager::GetInstance()->GetPath() + parts[1]);
   if (szFolderPath.empty()) {
      PLOGW.printf("Playlist folder not found: %s", parts[1].c_str());
      return nullptr;
   }

   bool hasFiles = false;
   for (const auto& entry : std::filesystem::directory_iterator(szFolderPath)) {
      if (entry.is_regular_file()) {
         string szFilename = entry.path().filename();
         if (!szFilename.empty() && szFilename[0] != '.') {
            hasFiles = true;
            break;
         }
      }
   }

   if (!hasFiles) {
      // TODO add to a pup pack audit, we log as info as not a big deal.
      PLOGI.printf("Playlist folder %s is empty",szFolderPath.c_str());
   }

   string szFolder = std::filesystem::path(szFolderPath).parent_path().filename().string();

   PUPPlaylist* pPlaylist = new PUPPlaylist(
      szFolder,
      parts[2],
      (string_to_int(parts[3], 0) == 1),
      string_to_int(parts[4], 0),
      string_to_int(parts[5], 0),
      string_to_int(parts[6], 0));

   return pPlaylist;
}

const string& PUPPlaylist::GetPlayFile(const string& szFilename)
{
   static string emptyString = "";

   std::map<string, string>::iterator it = m_fileMap.find(lowerCase(szFilename));
   return it != m_fileMap.end() ? it->second : emptyString;
}

const string& PUPPlaylist::GetNextPlayFile()
{
   if (!m_randomize) {
      const string& file = m_files[m_lastIndex];
      if (++m_lastIndex >= m_files.size())
         m_lastIndex = 0;
      return file;
   }
   return m_files[rand() % m_files.size()];
}

string PUPPlaylist::GetPlayFilePath(const string& szFilename)
{
   static string emptyString = "";

   if (m_files.empty())
      return emptyString;

   if (!szFilename.empty()) {
      std::map<string, string>::const_iterator it = m_fileMap.find(lowerCase(szFilename));
      if (it != m_fileMap.end())
         return m_szBasePath + it->second;
      else
         return emptyString;
   }
   else
      return m_szBasePath + GetNextPlayFile();
}

string PUPPlaylist::ToString() const {
   return "folder=" + m_szFolder +
      ", description=" + m_szDescription +
      ", randomize=" + (m_randomize ? "true" : "false") +
      ", restSeconds=" + std::to_string(m_restSeconds) +
      ", volume=" + std::to_string(m_volume) +
      ", priority=" + std::to_string(m_priority) +
      ", function=" + string(PUP_PLAYLIST_FUNCTION_TO_STRING(m_function));
}
