#include "stdafx.h"
#include "RssManagerException.h"
using namespace std;

namespace rssWork
{
	enum itcolums{ ITID = 0, ITTITLE, ITDESC, ITDATE, ITPARLINK };
	enum chcolums{ CHLINK = 0, CHTITLE, CHDESC, CHDATE };
	struct RssItem
	{
		wstring title;
		wstring pubDate;
		wstring desc;
		wstring channellink;
	};
	struct FeedModel
	{
		wstring title;
		wstring link;
		wstring desc;
		vector<RssItem>items;
		wstring lastBuildDate;
	};
	class RssManager
	{
	public:
		RssManager();
		~RssManager();
		FeedModel AddChannel(const string& url);
		vector<FeedModel> GetChannels()const;
		void Prepare();
		vector<FeedModel> ReadChannels();
		vector<FeedModel> GetOld();
		void RemoveChannel(const wstring& url);
	private:
		vector<FeedModel>channels_;
		wstring dbName = L"rssChannels.dblite";
	private:
		FeedModel ParsChannel(const wstring& page, const string&url);
		void ParsItems(const wstring&page,  FeedModel& channel);
		const wstring GetHtml(const string& url);
		static const size_t ReadCall(const char * data, const size_t size, const size_t nmemb, wstring& buff);
		int SaveChannal(const FeedModel& channel);
		void SaveItems(const vector<RssItem>&items);
		wstring CreateChparam(const string&query, const FeedModel& channel);
		wstring CreateITparam(const string& query, const RssItem&item);
		bool IsTable(const string& tableName);
		void AdditionItem( FeedModel& url);
	};
}
