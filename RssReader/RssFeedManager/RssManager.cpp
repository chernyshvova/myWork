#include "stdafx.h"
#include "RssManager.h"
#include "SqliteWrap.h"
using namespace rssWork;
using namespace pugi;

RssManager::RssManager()
{
}
RssManager::~RssManager(){}
FeedModel RssManager::ParsChannel(const wstring& page, const string& url)
{
	FeedModel model;
	xml_document doc;
	if (!doc.load(page.c_str()))
	{
		throw RssMNGException("Error load stringXML");
	}
	xml_node pointer = doc.child(L"rss").child(L"channel");

	model.title = pointer.child_value(L"title");
	model.link = wstring(url.cbegin(),url.cend());
	model.desc = pointer.child_value(L"description");
	model.lastBuildDate = pointer.child_value(L"lastBuildDate");
	
	this->ParsItems(page,model);
	return model;
}
void RssManager::ParsItems(const wstring&page,  FeedModel& channel)
{
	xml_document doc;
	if (!doc.load(page.c_str()))
	{
		throw RssMNGException("Error load_stringXML");
	}
	xml_node pointer = doc.child(L"rss").child(L"channel");
	string url(channel.link.cbegin(), channel.link.cend());

	for (xml_node i = pointer.child(L"item"); i; i = i.next_sibling(L"item"))
	{
		RssItem item;

		item.title = i.child_value(L"title");
		item.pubDate = i.child_value(L"pubDate");
		item.desc = i.child_value(L"description");
		item.channellink = channel.link;
		channel.items.push_back(item);
	}
}
const wstring RssManager::GetHtml(const string& url)
{
	wstring result = L"";
	string errbuf;
	shared_ptr<CURL> descr(curl_easy_init(), curl_easy_cleanup);
	CURLcode res;
	if (descr.get())
	{
		curl_easy_setopt(descr.get(), CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(descr.get(), CURLOPT_URL, url.c_str());
		curl_easy_setopt(descr.get(), CURLOPT_WRITEFUNCTION, ReadCall);
		curl_easy_setopt(descr.get(), CURLOPT_WRITEDATA, &result);
		res = curl_easy_perform(descr.get());

		if (res != CURLE_OK)
		{
			throw RssMNGException(curl_easy_strerror(res));
		}
	}
	return result;
}
const size_t RssManager::ReadCall(const char * data, const size_t size, const size_t nmemb, wstring& buff)
{
	size_t result = 0;
	if (&buff != NULL)
	{
		int len;
		len = MultiByteToWideChar(CP_UTF8, 0, data, nmemb, 0, false);
		vector<wchar_t> wbuff(len, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, data, nmemb, &wbuff[0], len);

		wstring result_str(wbuff.cbegin(), wbuff.cend());
		buff = result_str;
	}
	return size* nmemb;;
}
int RssManager::SaveChannal(const FeedModel& model)
{
		SqliteWrap db(this->dbName.c_str());
		const wstring querNewTb = L"CREATE TABLE IF NOT EXISTS tb_channels(link PRIMARY KEY UNIQUE  NOT NULL,title,desc,lastdate);";
		db.GetQuery(querNewTb);

		const wstring sqlinschannel = this->CreateChparam("INSERT INTO tb_channels(link,title,desc,lastdate) VALUES", model);
		db.GetQuery(sqlinschannel);

	return 0;
}
void RssManager::SaveItems(const vector<RssItem>& items)
{
	SqliteWrap db(this->dbName.c_str());
	wstring crTbSql = L"CREATE TABLE IF NOT EXISTS tb_items(id INTEGER PRIMARY KEY AUTOINCREMENT,title,desc,pubDate,channellink,FOREIGN KEY (channellink) REFERENCES tb_channels(link))";

	db.GetQuery(crTbSql);
	for (RssItem it : items)
	{
		wstring insertSql = this->CreateITparam("INSERT INTO tb_items(title, desc, pubDate, channellink)VALUES", it);
		db.GetQuery(insertSql);
	}
}
vector<FeedModel> RssManager::ReadChannels()
{
	vector<FeedModel> result;
	const wstring queryGetchannels = L"SELECT * FROM tb_channels;";

	SqliteWrap db(this->dbName.c_str());
	vector<vector<wstring>> colums = db.GetQueryResultWString(queryGetchannels);

	for (int i = 0; i < colums.size(); ++i)
	{
		FeedModel channel;
		channel.link = colums[i][CHLINK];
		channel.title = colums[i][CHTITLE];
		channel.desc = colums[i][CHDESC];
		channel.lastBuildDate = colums[i][CHDATE];
		result.push_back(channel);
	}

	for (int i = 0; i < result.size(); ++i)
	{
		string url(result[i].link.cbegin(), result[i].link.cend());
		wstring page = this->GetHtml(url);

		this->ParsItems(page, result[0]);
	}
	return result;
}
wstring RssManager::CreateITparam(const string& query, const RssItem&item)
{
	wstring del = L"','";
	wstring result = wstring(L"('") + item.title.c_str() + del + item.desc.c_str() + del;
	result += item.pubDate.c_str() + del + item.channellink.c_str() + L"');";
	
	result = wstring(query.begin(),query.end()) + result;
	return result;
}
wstring RssManager::CreateChparam(const string& query, const FeedModel& ch)
{	
	wstring del = L"','";
	wstring result = wstring(L"('") + ch.link.c_str() + del + ch.title.c_str() + del + ch.desc.c_str() + del + ch.lastBuildDate.c_str() + L"');";
	result = wstring(query.begin(),query.end()) + result;
	
	return result;
}
 vector<FeedModel> RssManager::GetChannels()const
{
	return this->channels_;
}
FeedModel RssManager::AddChannel(const string& url)
{
	wstring page = this->GetHtml(url);
	FeedModel channel = this->ParsChannel(page, url);

	this->SaveChannal(channel);
	this->SaveItems(channel.items);

	this->channels_.push_back(channel);
	return channel;
}	
void RssManager::AdditionItem(FeedModel& mychannel)
{
	string url(mychannel.link.cbegin(), mychannel.link.cend());
	wstring page = this->GetHtml(url);
	xml_document doc;
	if (!doc.load(page.c_str()))
	{
		throw RssMNGException("Error load stringXML");
	}
	xml_node pointer = doc.child(L"rss").child(L"channel");

	wstring lastDate = pointer.child_value(L"lastBuildDate");
	
	if (lastDate == mychannel.lastBuildDate.c_str())
	{
		return;
	}
	vector<RssItem> newItems;
	for (xml_node i = pointer.child(L"item"); i; i = i.next_sibling(L"item"))
	{
		RssItem item;

		item.title = i.child_value(L"title");
		item.pubDate = i.child_value(L"pubDate");
		item.desc = i.child_value(L"description");
		item.channellink = mychannel.link;
		mychannel.items.push_back(item);
	}
	
	this->SaveItems(newItems);
}
void RssManager::RemoveChannel(const wstring& url)
{
	const wstring sql_channels = L"DELETE FROM tb_channels WHERE link='" + url + L"';";
	const wstring sql_items = L"DELETE FROM tb_items WHERE channellink='" + url + L"';";

	SqliteWrap db(this->dbName);
	db.GetQuery(sql_items);
	db.GetQuery(sql_channels);
	
	for (int i = 0; i < this->channels_.size(); ++i)
	{
		if (this->channels_[i].link == url)
			this->channels_.erase(this->channels_.begin() + i);
	}

}
void RssManager::Prepare()
{
	vector<FeedModel> channels;
	
	if (!this->IsTable("tb_channels"))
	{
		return;
	}

	channels = this->ReadChannels();
	for (FeedModel ch : channels)
	{
		this->AdditionItem(ch);
	}

	this->channels_ = channels;
}
bool RssManager::IsTable(const string& tableName)
{
	wstring checkQR = L"SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = 'tb_channels'";
	
	SqliteWrap conx(this->dbName);
	vector<vector<wstring>> result =conx.GetQueryResultWString(checkQR);

	if (result[0][0][0] == L'0')
		return false;
	else
		return true;
	
}
vector<FeedModel> RssManager::GetOld()
{
	vector<FeedModel> channels;
	const wstring queryGetchannels = L"SELECT * FROM tb_channels;";

	SqliteWrap db(this->dbName.c_str());
	vector<vector<wstring>> colums = db.GetQueryResultWString(queryGetchannels);

	for (int i = 0; i < colums.size(); ++i)
	{
		FeedModel channel;
		channel.link = colums[i][CHLINK];
		channel.title = colums[i][CHTITLE];
		channel.desc = colums[i][CHDESC];
		channel.lastBuildDate = colums[i][CHDATE];
		channels.push_back(channel);
	}

	for (int i = 0; i < channels.size(); ++i)
	{
		const wstring queryGetitems = L"Select * from tb_items where channellink='" + channels[i].link +L"';";
		
		vector<vector<wstring>> items= db.GetQueryResultWString(queryGetitems);

		for (int j = 0; j < items.size(); ++j)
		{
			RssItem item;
			item.title = items[j][ITTITLE];
			item.desc = items[j][ITDESC];
			item.pubDate = items[j][ITDATE];
			item.channellink = items[j][ITPARLINK];
			channels[i].items.push_back(item);
		}
	}
	return channels;
}