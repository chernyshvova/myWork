// RssReader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../RssFeedManager/RssManager.h"
#include <iostream>

using namespace rssWork;
using namespace std;
void AddChannel(RssManager&conx);
void Show(FeedModel& channel);
void Show(const vector<FeedModel>&channels, RssManager& mng);
FeedModel Select(RssManager& conx);
void ShowOld(RssManager& conx);
void RemoveChannel(RssManager& conx);
int _tmain(int argc, _TCHAR* argv[])
{

		RssManager conx;


		setlocale(LC_ALL, "rus");
		try
		{
			conx.Prepare();
		}
		catch (exception ex)
		{

		}
		
		while (true)
		{
			cout << "1.Exit" << endl;
			cout << "2.addChannal" << endl;
			cout << "3.Show" << endl;
			cout << "4.OldNews" << endl;
			cout << "5.Remove" << endl;
			int vib = 0;
			try
			{
				cin >> vib;

				switch (vib)
				{
				case 1:
					return 0;
				case 2:
					AddChannel(conx);
					break;
				case 3:
					Show(Select(conx));
					break;
				case 4:
					ShowOld(conx);
				case 5:
					RemoveChannel(conx);
					break;
				default:
					throw std::exception("Wrong param");
				}
			}
			catch (const RssBdException& ex)
			{
				ex.what();
			}
			catch (const RssMNGException& ex)
			{
				ex.what();
			}
			catch (exception& ex)
			{
				cout << ex.what() << "\n";
			}
		}
	return 0;
}
void AddChannel(RssManager& conx)
{
	cout << "enter url" << endl;
	string url;
	cin >> url;

	FeedModel channel = conx.AddChannel(url);
	cout << "ok" << endl;
	Show(channel);
}
void Show(FeedModel& channel)
{
	wcout << "-----------------------------------------------------------" << endl;
	wcout << L"Link: " << channel.link.c_str() << endl << endl;
	wcout << L"Title: " << channel.title.c_str() << endl << endl;
	wcout << L"Desctiption: " << channel.desc.c_str() << endl << endl;
	wcout << "-----------------------------------------------------------" << endl;
	for (RssItem it : channel.items)
	{
		wcout << L"Title: " << it.title.c_str()<< endl;
		wcout << L"Date: " <<it.pubDate.c_str() << endl;
		wcout << L"Desctiption: " << it.desc.c_str() << endl;
		cout << endl << endl;
	}
}
void Show(const vector<FeedModel>&channels, RssManager& mng)
{
	for (FeedModel feed : channels)
	{
		Show(feed);
		cout << endl<<endl;
	}
}
FeedModel Select(RssManager& conx)
{
	vector<FeedModel> channels = conx.GetChannels();
	
	if (channels.size() == 0)
	{
		throw exception("no such channels");
	}
	for (int i = 0; i < channels.size(); ++i)
	{
		wcout <<i<< L":"<< channels[i].title << endl;
	}

	cout << endl << "Select ¹" << endl;
	int vib;
	cin >> vib;

	if (vib>channels.size()||vib<0)
	{
		throw exception("Selected wrong channel");
	}
	return channels[vib];
}
void ShowOld(RssManager& conx)
{
	vector<FeedModel> channels = conx.GetOld();
	Show(channels, conx);
}
void RemoveChannel(RssManager& conx)
{
	FeedModel ch = Select(conx);
	conx.RemoveChannel(ch.link);
}