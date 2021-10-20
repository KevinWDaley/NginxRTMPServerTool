#include <menu.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <ctime>
#include <numeric>
#include <unistd.h>
#include <vector>
#include <regex>

//My header files for functions...
#include "nginx-process-id.h"
#include "ngnix-ip-address.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

using namespace std;
string ip_Address = "";
string bitrate = "";
string resolution = "";
string framerate = "";
string cpuPreset = "";
string streamService = "";
string ingestServer = "";
string ingestServerName = "";
string streamKey = "";
string streamKeyActive = "";
size_t previous_idle_time=0, previous_total_time=0;
size_t previous_network_data_transmit = 0;
size_t previous_network_data_receive = 0;

const char *appName = "Nginx RTMP Server Tool";

const char *choices[] = {

                        "Turn ON",
                        "Turn OFF",
                        "Reload CONF",
                        "Bitrate",
                        "Framerate",
                        "CPU Preset",
                        "Resolution",
                        "Stream Service",
                        "Ingest Server",
                        "Stream Key",
                        "EXIT",
                        (char *)NULL,
                  };

bool currentOption(  const char* input, WINDOW *my_menu_win, MENU *my_menu );
void set_IP_Address();
void draw_menu(  WINDOW *my_menu_win, MENU *my_menu);
void print_in_middle(WINDOW *win, int starty, int startx, int width, const char *string, chtype color);
void changeSetting( const char* input, WINDOW *my_menu_win, const char* choices );
void changeIngestServer(const char* input, WINDOW *my_menu_win, const char* choices );
void printSettings( WINDOW *my_menu_win );
void currentSettingIngest( WINDOW *my_menu_win );
void currentSetting( WINDOW *my_menu_win );
void checkProcess(WINDOW *my_menu_win);
float get_cpu_usage();
float get_network_usage_transmit();
float get_network_usage_receive();
float get_memory_usage();
string get_PID_ngnix();
string get_IP_Address();
string get_ingest_server( string choice, string streamService );
string get_streamKey( string str );

int main()
{
    ITEM **my_items;
    int c;
    MENU *my_menu;
    WINDOW *my_menu_win;
    int n_choices, i;
    bool loop = true;
    ip_Address = get_IP_Address();

	/* Initialize curses */
	initscr();
	start_color();
    cbreak();
    noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	init_pair(5, COLOR_WHITE, COLOR_BLACK);
	/* Create items */
    n_choices = ARRAY_SIZE(choices);
    my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
    for(i = 0; i < n_choices; ++i)
    my_items[i] = new_item(choices[i], 0 );

	/* Crate menu */
	my_menu = new_menu((ITEM **)my_items);

	/* Create the window to be associated with the menu */
    my_menu_win = newwin(28, 75, 4, 4);
    keypad(my_menu_win, TRUE);

	/* Set main window and sub window */
    set_menu_win(my_menu, my_menu_win);
    set_menu_sub(my_menu, derwin(my_menu_win, 11, 38, 5, 18));


	/* Set menu mark to the string " * " */
    set_menu_mark(my_menu, " -> ");

    set_IP_Address();
    draw_menu( my_menu_win, my_menu);
    currentSetting( my_menu_win );
    currentSettingIngest( my_menu_win);
    printSettings(my_menu_win );
    nodelay( my_menu_win, true );
	wtimeout( my_menu_win, 1000 );

	while( loop != false && (c = wgetch(my_menu_win))  )
	{
        switch(c)
        {	case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			case 10:
                nodelay( my_menu_win, false );
				loop = currentOption( item_name(current_item(my_menu)), my_menu_win, my_menu );
                nodelay( my_menu_win, true );
                wtimeout( my_menu_win, 1000 );
				break;
            case ERR:
                checkProcess(my_menu_win);
                break;
        }


        draw_menu( my_menu_win, my_menu);
        printSettings( my_menu_win );
        //currentSetting( my_menu_win );
        wrefresh(my_menu_win);
        refresh();
	}

	/* Unpost and free all the memory taken up */
	curs_set(1);
    unpost_menu(my_menu);
    free_menu(my_menu);
    for(i = 0; i < n_choices; ++i)
        free_item(my_items[i]);
	endwin();
	clear();
	exit( EXIT_SUCCESS );
}

string get_IP_Address()
{
    char buffer[15] = {'\0'};
    char ipAddress[15] = {'\0'};
    string temp = "";
    shared_ptr<FILE> pipe(popen("hostname -I","r"), pclose);
    if (!pipe) throw runtime_error("popen() failed!");

    int i = 0;
  while (!feof(pipe.get()))
  {
      buffer[i++] = fgetc(pipe.get());
  }
  //buffer[14] = '+';

  for( int i = 0; i < 15; i++)
  {
      if( buffer[i] == '1' || buffer[i] == '2' || buffer[i] == '3' || buffer[i] == '4' || buffer[i] == '5' || buffer[i] == '6' || buffer[i] == '7' || buffer[i] == '8' || buffer[i] == '9' || buffer[i] == '.' )
      {
          ipAddress[i] = buffer[i];
      }

  }


  return ipAddress;
}

void draw_menu( WINDOW *my_menu_win, MENU *my_menu )
{
    /* Print a border around the main window and print a title */
    box(my_menu_win, 0, 0);
    print_in_middle(my_menu_win, 1, 0, 75, appName, COLOR_PAIR(1));
    mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
    mvwaddch(my_menu_win, 24, 0, ACS_LTEE);
    mvwhline(my_menu_win, 2, 1, ACS_HLINE, 75);
    mvwhline(my_menu_win, 24, 1, ACS_HLINE, 75);
    mvwaddch(my_menu_win, 2, 74, ACS_RTEE);
    mvwaddch(my_menu_win, 24, 74, ACS_RTEE);
    refresh();

    /* Post the menu */
    post_menu(my_menu);
    wrefresh(my_menu_win);
    mvwprintw( my_menu_win, 3, 3, "NGINX SERVICE:                     Server Statistics" );

    wattron( my_menu_win ,COLOR_PAIR(2));
    mvwprintw( my_menu_win, 5, 3, "Server Options" );

    int i;
    for( i = 0; i < 3; ++i )
    {
        mvwprintw( my_menu_win, (5 + i), 17, "|" );
    }
    wattroff( my_menu_win, COLOR_PAIR(2));

    wattron( my_menu_win, COLOR_PAIR(3));
    mvwprintw( my_menu_win, 8, 3, "Encode Options" );
    for( i = 0; i < 4; ++i )
    {
        mvwprintw( my_menu_win, (8 + i), 17, "|" );
    }
    wattroff( my_menu_win, COLOR_PAIR(3));

    wattron( my_menu_win, COLOR_PAIR(4));
    mvwprintw( my_menu_win, 12, 3, "Stream Options" );
    for( i = 0; i < 3; ++i )
    {
        mvwprintw( my_menu_win, (12 + i), 17, "|" );
    }
    wattroff( my_menu_win, COLOR_PAIR(4));

    wattron( my_menu_win, COLOR_PAIR(5));
    mvwprintw( my_menu_win, 15, 3, "Exit Program  " );
    mvwprintw( my_menu_win, 15, 17, "|" );
    wattroff( my_menu_win, COLOR_PAIR(5));

    if( nginxPID() == false )
    {
        mvwprintw( my_menu_win, 3, 17, " OFF    " );
    }
	else if( nginxPID() == true )
	{
        mvwprintw( my_menu_win, 3, 17, " ON    " );
	}
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, const char *string, chtype color)
{	int length, x, y;
	float temp;

	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;

	length = strlen(string);
	temp = (width - length)/ 2;
	x = startx + (int)temp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);
	refresh();
}

bool currentOption( const char* input, WINDOW *my_menu_win, MENU *my_menu )
{
	if( strcmp( input, choices[0] ) == 0 )
	{
		if( nginxPID() == false )
		{
			system( "/usr/local/nginx/sbin/./nginx" );
			mvwprintw( my_menu_win, 3, 17, " ON                 " );
		}
	}
	else if( strcmp( input, choices[1] ) == 0 )
	{

		if( nginxPID() == true )
		{
			system( "/usr/local/nginx/sbin/./nginx -s stop" );
			mvwprintw( my_menu_win, 3, 17, " OFF                " );
		}


	}
	else if( strcmp( input, choices[2] ) == 0 )
	{
		if( nginxPID() == true )
		{
			system( "./nginx -s stop" );
			system( "./nginx" );
			mvwprintw( my_menu_win, 3, 17, " ON - CONF Reloaded" );
		}
	}
	else if( strcmp( input, choices[3] ) == 0 )
	{
		changeSetting( input, my_menu_win, choices[3] );
	}
	else if( strcmp( input, choices[4] ) == 0 )
	{
		changeSetting( input, my_menu_win, choices[4] );
	}
	else if( strcmp( input, choices[5] ) == 0 )
	{
		changeSetting( input, my_menu_win, choices[5] );
	}
 	else if( strcmp( input, choices[6] ) == 0 )
	{
		changeSetting( input, my_menu_win, choices[6] );
	}
    else if( strcmp( input, choices[7] ) == 0 )
	{
        changeIngestServer( input, my_menu_win, choices[7] );
	}
    else if( strcmp( input, choices[8] ) == 0 )
	{
        changeIngestServer( input, my_menu_win, choices[8] );
	}
    else if( strcmp( input, choices[9] ) == 0 )
	{
        changeIngestServer( input, my_menu_win, choices[9] );
	}
	else if( strcmp( input, choices[10] ) == 0 )
	{
		if( nginxPID() == true )
		{
			system( "./nginx -s stop" );
		}
		return false;
	}
	return true;
}

void checkProcess(WINDOW *my_menu_win)
{
     //get_PID_ngnix(my_menu_win);

     if( nginxPID() == false )
     {
         mvwprintw( my_menu_win, 3, 17, " OFF                " );
     }

    mvwprintw( my_menu_win, 5, 38, "| PID:       %s", get_PID_ngnix().c_str() );
    mvwprintw( my_menu_win, 6, 38, "| CPU:     %8.3f %%", get_cpu_usage() );
    mvwprintw( my_menu_win, 7, 38, "| MEM:     %8.3f GB", get_memory_usage() );
    mvwprintw( my_menu_win, 8, 38, "| NET/T:   %8.3f", get_network_usage_transmit() );
    mvwprintw( my_menu_win, 8, 57, " Kbps" );
    mvwprintw( my_menu_win, 9, 38, "| NET/R:   %8.3f", get_network_usage_receive() );
    mvwprintw( my_menu_win, 9, 57, " Kbps" );
    mvwprintw( my_menu_win, 10, 38, "| L-IP:    %s", ip_Address.c_str() );
}

void printSettings( WINDOW *my_menu_win )
{
    mvwprintw( my_menu_win, 12, 39, "X264 Settings" );
    mvwprintw( my_menu_win, 14, 38, "| CPU Preset:       %s", cpuPreset.c_str() ) ;
    mvwprintw( my_menu_win, 15, 38, "| Bitrate:          %s", bitrate.c_str() ) ;
    mvwprintw( my_menu_win, 16, 38, "| Resolution:       %s", resolution.c_str() ) ;
    mvwprintw( my_menu_win, 17, 38, "| Framerate:        %s", framerate.c_str() ) ;
    mvwprintw( my_menu_win, 19, 39, "Stream Settings" );
    mvwprintw( my_menu_win, 21, 38, "| Stream Service: %s", streamService.c_str() ) ;
    mvwprintw( my_menu_win, 22, 38, "| Ingest Server:  %s", ingestServerName.c_str() ) ;
    mvwprintw( my_menu_win, 23, 38, "| Stream Key:     %s", streamKeyActive.c_str() ) ;
}

void currentSettingIngest( WINDOW *my_menu_win )
{
    ifstream conf;
    conf.open( "/usr/local/nginx/conf/nginx.conf" );
	string str = "";
    string temp = "";
    bool flag = false;

    while( conf >> str )
	{

        if( strstr( str.c_str(), "rtmp://live-ams.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Amsterdam";
            ingestServer = "rtmp://live-ams.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
            mvwprintw( my_menu_win, 25, 23, "Here!" ) ;
        }
        if( strstr( str.c_str(), "rtmp://live-arn.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Stockholm";
            ingestServer = "rtmp://live-arn.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-cdg.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Paris";
            ingestServer = "rtmp://live-cdg.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-dfw.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Dallas";
            ingestServer = "rtmp://live-dfw.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-eze.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Argentina";
            ingestServer = "rtmp://live-eze.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-fra.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Frankfurt";
            ingestServer = "";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-gig.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Rio de Janeiro";
            ingestServer = "";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-gru.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Sao Paulo";
            ingestServer = "";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-hkg.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Hong Kong";
            ingestServer = "";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-iad.twitch.tv/app" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Ashburn";
            ingestServer = "";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-jfk.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "New York";
            ingestServer = "";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-lax.twitch.tv/app/" )  )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Los Angeles";
            ingestServer = "rtmp://live-lax.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-lhr.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "London";
            ingestServer = "rtmp://live-lhr.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-mia.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Miami";
            ingestServer = "rtmp://live-mia.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-ord.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Chicago";
            ingestServer = "rtmp://live-ord.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-prg.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Prague";
            ingestServer = "rtmp://live-prg.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-scl.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Chile";
            ingestServer = "rtmp://live-scl.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-sea.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Seattle";
            ingestServer = "rtmp://live-sea.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-sel.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Seoul";
            ingestServer = "rtmp://live-sel.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-sin.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Singapore";
            ingestServer = "rtmp://live-sin.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-sjc.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "San Jose";
            ingestServer = "rtmp://live-sjc.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-syd.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Sydney";
            ingestServer = "rtmp://live-syd.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-tpe.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Taipei";
            ingestServer = "rtmp://live-tpe.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-tyo.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Tokyo";
            ingestServer = "rtmp://live-tyo.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live-waw.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "Warsaw";
            ingestServer = "rtmp://live-waw.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if( strstr( str.c_str(), "rtmp://live.twitch.tv/app/" ) )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "San Francisco";
            ingestServer = "rtmp://live.twitch.tv/app/";
            streamKeyActive = "ACTIVE  ";
            flag = true;
        }
        if(flag == false && streamService.compare("Twitch") == 0 )
        {
            streamKey = get_streamKey(str);
            streamService = "Twitch";
            ingestServerName = "NULL";
            ingestServer = "NULL";
            streamKeyActive = "INACTIVE";
        }
        if(flag == false && streamService.compare("Twitch") != 0 )
        {
            streamKey = get_streamKey(str);
            streamService = "NULL";
            ingestServerName = "NULL";
            ingestServer = "NULL";
            streamKeyActive = "INACTIVE";
        }
    }

    conf.close();
}

void currentSetting( WINDOW *my_menu_win )
{
	ifstream conf;
    conf.open( "/usr/local/nginx/conf/nginx.conf" );
	string str = "";
    string temp = "";
	while( conf >> str )
	{

        if( temp.compare("-preset") == 0  )
        {
            cpuPreset = str;
            temp = "";
        }
        if( temp.compare("-b:v") == 0  )
        {
            bitrate = str;
            temp = "";
        }
        if( temp.compare("-s") == 0  )
        {
            resolution = str;
            temp = "";
        }
        if( temp.compare("-r") == 0  )
        {
            framerate = str;
            temp = "";
        }


		if( str.compare("-preset") == 0 )
		{
			temp = str;
		}
        if( str.compare("-b:v") == 0 )
        {
            temp = str;
        }
        if(str.compare("-s") == 0 )
        {
            temp = str;
        }
        if( str.compare("-r") == 0 )
        {
            temp = str;
        }
	}
	conf.close();

}

void set_IP_Address()
{
    ifstream conf;
	ifstream conf_lines;
    int lines = 0;
    ofstream editSettings;
    string str;
	string avconv;
    string temp;
    int avconvLocation;
    char c;

    temp = "/"+ip_Address+"/";

    conf_lines.open( "/usr/local/nginx/conf/nginx.conf" );
    while (conf_lines.get(c))
    {
    if (c == '\n')
        lines++;

    }
    conf_lines.close();

    conf.open( "/usr/local/nginx/conf/nginx.conf" );
    if( !conf.is_open() )
    {
        //mvwprintw( my_menu_win, 20, 5, " file is not open ");
    }

    string tempfile[lines];

    int i=0;
	while( getline( conf, str ) )
	{
		tempfile[i++] = str;

		if( strstr( str.c_str(), "exec") )
		{
			avconv = str;
			avconvLocation = i;
		}
	}

    regex ipAddress_style( "\\/[[:digit:]]{1,3}\\.[[:digit:]]{1,3}\\.[[:digit:]]{1,3}\\.[[:digit:]]{1,3}\\/" );
    avconv = regex_replace( avconv, ipAddress_style, temp );
    tempfile[avconvLocation-1] = avconv;

    editSettings.open( "/usr/local/nginx/conf/nginx.conf" );

	for( int j = 0; lines > j; j++ )
	{
		editSettings << tempfile[j] << endl;
	}
	editSettings.close();



}

void changeIngestServer(const char* input, WINDOW *my_menu_win, const char* choices )
{
    ifstream conf;
	ifstream conf_lines;
	vector<string> temp_file;
	int ingestLocation;




	string str;
	string ingest;
    string new_ingest_server;
	char choice[64] = {'\0'};

    ofstream editSettings;
    int lines = 0;
    int arguments = 0;
    char c;

    conf.open( "/usr/local/nginx/conf/nginx.conf" );
	if( !conf.is_open() )
	{
		mvwprintw( my_menu_win, 23, 3, " file is not open ");
	}

    conf_lines.open( "/usr/local/nginx/conf/nginx.conf" );
    while (conf_lines.get(c))
    {
    if (c == '\n')
        lines++;

    }
    conf_lines.close();

    string tempfile[lines];

    int i=0;
	while( getline( conf, str ) )
	{
		tempfile[i++] = str;

		if( strstr( str.c_str(), "push") )
		{
			ingest = str;
			ingestLocation = i;
		}
	}

	istringstream args(ingest);

    if( choices == "Stream Service" )
    {
        mvwprintw( my_menu_win, 25, 3, "New %s", choices );
        mvwprintw( my_menu_win, 26, 3, "Enter: " );
        curs_set(2);
        echo();
        bool service = false;

        wgetstr( my_menu_win, choice );
        curs_set(0);
        noecho();

        if( strcmp(choice, "Twitch" ) == 0 || strcmp(choice,  "twitch" ) == 0 )
        {
            service = true;
            choice[0] = 'T';
            streamService = choice;
            mvwprintw( my_menu_win, 25, 3, "                              " );
            mvwprintw( my_menu_win, 26, 3, "                              " );

            ingestServer = "";
            ingestServerName = "NULL";
            streamKey = "";
            streamKeyActive = "INACTIVE";
            mvwprintw( my_menu_win, 25, 3, "[%s]", streamService.c_str() );
        }
        if( strcmp(choice, "Beam" ) == 0 || strcmp(choice,  "beam" ) == 0 )
        {
            service = true;
            choice[0] = 'B';
            streamService = choice;
            mvwprintw( my_menu_win, 25, 3, "                              " );
            mvwprintw( my_menu_win, 26, 3, "                              " );

            ingestServer = "";
            ingestServerName = "NULL";
            streamKey = "";
            streamKeyActive = "INACTIVE";
        }
        if( service == false )
        {
            mvwprintw( my_menu_win, 25, 3, "                              " );
            mvwprintw( my_menu_win, 26, 3, "INVALID INPUT!                " );
        }

    }

    if( choices == "Ingest Server" )
    {
        string temp1;
        string temp2;
        mvwprintw( my_menu_win, 25, 3, "New %s", choices );
        mvwprintw( my_menu_win, 26, 3, "Enter: " );
        curs_set(2);
        echo();


        wgetstr( my_menu_win, choice );
        curs_set(0);
        noecho();

        string temp(choice);
        temp1 = ingestServer;
        temp2 = ingestServerName;
        ingestServer = get_ingest_server( temp, streamService );

        if( ingestServer.compare( "NULL" ) == 0)
        {
            ingestServer = temp1;
            ingestServerName = temp2;
            mvwprintw( my_menu_win, 25, 3, "                              " );
            mvwprintw( my_menu_win, 26, 3, "INVALID INPUT!                " );

        }
        else
        {
            mvwprintw( my_menu_win, 25, 3, "                              " );
            mvwprintw( my_menu_win, 26, 3, "                              " );
        }

    }

    if( choices == "Stream Key" )
    {
        mvwprintw( my_menu_win, 25, 3, "New %s", choices );
        mvwprintw( my_menu_win, 26, 3, "Enter: " );
        curs_set(2);
        echo();


        wgetstr( my_menu_win, choice );
        curs_set(0);
        noecho();

        string temp(choice);
        streamKey = temp;
        streamKeyActive = "Active";
        mvwprintw( my_menu_win, 25, 3, "                              " );
        mvwprintw( my_menu_win, 26, 3, "                                                       ");

    }


    new_ingest_server = "push " + ingestServer + streamKey + ";";

    tempfile[ingestLocation-1] = new_ingest_server;

    editSettings.open( "/usr/local/nginx/conf/nginx.conf" );

	for( int j = 0; lines > j; j++ )
	{
		editSettings << tempfile[j] << endl;
	}
	editSettings.close();



    currentSettingIngest( my_menu_win );

}

void changeSetting( const char* input, WINDOW *my_menu_win, const char* choices )
{

	ifstream conf;
	ifstream conf_lines;
	vector<string> temp_file;
	int avconvLocation;

	string str;
	string avconv;
	char choice[9];
	//string avconvSettings[45];
    ofstream editSettings;
    int bitrate_location;
    int minrate_location;
    int maxrate_location;
    int framerate_location;
    int cpupreset_location;
    int resolution_location;
    int lines = 0;
    int arguments = 0;
    char c;



    conf_lines.open( "/usr/local/nginx/conf/nginx.conf" );
    while (conf_lines.get(c))
    {
    if (c == '\n')
        lines++;

    }
    conf_lines.close();

	conf.open( "/usr/local/nginx/conf/nginx.conf" );
	if( !conf.is_open() )
	{
		mvwprintw( my_menu_win, 20, 5, " file is not open ");
	}

    string tempfile[lines];

	int i=0;
	while( getline( conf, str ) )
	{
		tempfile[i++] = str;

		if( strstr( str.c_str(), "exec") )
		{
			avconv = str;
			avconvLocation = i;
		}
	}

	istringstream args(avconv);

	while( args >> str )
    {
        arguments++;
    }

    string avconvSettings[arguments];
	istringstream iss(avconv);
	i = 0;
	while( iss >> str )
	{

        if( str.compare("-r") == 0  ){
            framerate_location = i + 1;
	    }

        if( str.compare("-preset") == 0  ){
             cpupreset_location = i + 1;
	    }

        if( str.compare("-s") == 0  ){
             resolution_location = i + 1;
	    }

        if( str.compare("-b:v") == 0  ){
             bitrate_location = i + 1;
	    }

        if( str.compare("-minrate") == 0  ){
             minrate_location = i + 1;
	    }

        if( str.compare("-maxrate") == 0  ){
            maxrate_location = i + 1;
	    }

		avconvSettings[i++] = str;
	}


	if( choices == "Bitrate" )
	{
        mvwprintw( my_menu_win, 26, 3, "                              " );
        mvwprintw( my_menu_win, 25, 3, "New %s: ", choices );
        mvwprintw( my_menu_win, 26, 3, "Enter: " );
        curs_set(2);
        echo();


        wgetstr( my_menu_win, choice );
        curs_set(0);
        noecho();

        regex bitrate_style( "[[:digit:]]+[k]" );
        regex bitrate_style_alt( "[[:digit:]]+");

        if( regex_match( choice, bitrate_style ) )
        {

            mvwprintw( my_menu_win, 25, 3, "                              " );
            mvwprintw( my_menu_win, 26, 3, "                              " );
            avconvSettings[bitrate_location] = choice;
            avconvSettings[minrate_location] = choice;
            avconvSettings[maxrate_location] = choice;
        }
        else if( regex_match( choice, bitrate_style_alt ) )
        {

            string str(choice);
            str.append("k");
            strncpy( choice, str.c_str(), 9  );

            mvwprintw( my_menu_win, 25, 3, "                              " );
            mvwprintw( my_menu_win, 26, 3, "                              " );
            avconvSettings[bitrate_location] = choice;
            avconvSettings[minrate_location] = choice;
            avconvSettings[maxrate_location] = choice;

        }
        else
        {
            mvwprintw( my_menu_win, 25, 3, "                             " );
            mvwprintw( my_menu_win, 26, 3, "INVALID INPUT!               " );
        }


	}

    if( choices == "Framerate" )
    {
        mvwprintw( my_menu_win, 26, 3, "                              " );
        mvwprintw( my_menu_win, 25, 3, "New %s", choices );
        mvwprintw( my_menu_win, 26, 3, "Enter: " );
        curs_set(2);
        echo();

        wgetstr( my_menu_win, choice );
        curs_set(0);
        noecho();

        int temp;
        istringstream(choice) >> temp;

        if( temp > 0 && temp < 61 )
        {
            mvwprintw( my_menu_win, 25, 3, "                              " );
            mvwprintw( my_menu_win, 26, 3, "                              " );
            avconvSettings[framerate_location] = choice;

        }
        else
        {
            mvwprintw( my_menu_win, 25, 3, "                             " );
            mvwprintw( my_menu_win, 26, 3, "INVALID INPUT!               " );
        }



    }

    if( choices == "CPU Preset" )
    {
        mvwprintw( my_menu_win, 26, 3, "                              " );
        mvwprintw( my_menu_win, 25, 3, "New %s", choices );
        mvwprintw( my_menu_win, 26, 3, "Enter: " );
        curs_set(2);
        echo();

        wgetstr( my_menu_win, choice );
        curs_set(0);
        noecho();

        if( strcmp(choice, "slower" ) == 0 || strcmp(choice, "slow" ) == 0 || strcmp(choice, "medium" ) == 0 || strcmp(choice, "fast" ) == 0 || strcmp(choice, "faster" ) == 0 || strcmp(choice, "veryfast" ) == 0)
        {
            mvwprintw( my_menu_win, 25, 3, "                              " );
            mvwprintw( my_menu_win, 26, 3, "                              " );
            avconvSettings[cpupreset_location] = choice;
        }
        else
        {
            mvwprintw( my_menu_win, 25, 3, "                             " );
            mvwprintw( my_menu_win, 26, 3, "INVALID INPUT!               " );
        }


  }

  if( choices == "Resolution" )
  {
      regex resolution_style( "[[:digit:]]+[x][[:digit:]]+" );
      mvwprintw( my_menu_win, 26, 3, "                              " );
      mvwprintw( my_menu_win, 25, 3, "New %s", choices );
      mvwprintw( my_menu_win, 26, 3, "Enter: " );
      curs_set(2);
      echo();

      wgetstr( my_menu_win, choice );
      curs_set(0);
      noecho();


      if( regex_match( choice, resolution_style ) )
      {
          mvwprintw( my_menu_win, 25, 3, "                              " );
          mvwprintw( my_menu_win, 26, 3, "                              " );
          avconvSettings[resolution_location] = choice;
      }
      else
      {
          mvwprintw( my_menu_win, 25, 3, "                             " );
          mvwprintw( my_menu_win, 26, 3, "INVALID INPUT!               " );
      }

  }

	avconv = "";
	for( int k = 0; arguments > k; k++ )
	{
		avconv.append( avconvSettings[k] ).append( " " );
	}
	tempfile[avconvLocation-1] = avconv;

	editSettings.open( "/usr/local/nginx/conf/nginx.conf" );

	for( int j = 0; lines > j; j++ )
	{
		editSettings << tempfile[j] << endl;
	}
	editSettings.close();

    currentSetting( my_menu_win );
}

float get_network_usage_transmit()
{
    vector<string> network;
    ifstream proc_net("/proc/net/dev");
    size_t current_network;
    float final_network;
    for(string str; proc_net >> str; network.push_back(str));
    stringstream sstream(network[46]);
    sstream >> current_network;
    final_network = ((float)(current_network - previous_network_data_transmit)/1000) * 0.98;
    previous_network_data_transmit = current_network;

    return final_network;
}


float get_network_usage_receive()
{
    vector<string> network;
    ifstream proc_net("/proc/net/dev");
    size_t current_network;
    float final_network;
    for(string str; proc_net >> str; network.push_back(str));
    stringstream sstream(network[38]);
    sstream >> current_network;
    final_network = ((float)(current_network - previous_network_data_receive)/1000) * 0.98;
    previous_network_data_receive = current_network;

    return final_network;
}

float get_cpu_usage()
{
    vector<size_t> cpu;
    ifstream proc_stat("/proc/stat");
    proc_stat.ignore(5, ' '); // Skip the 'cpu' prefix
    for (size_t time; proc_stat >> time; cpu.push_back(time));
    size_t idle_time = cpu[3];
    size_t total_time = accumulate(cpu.begin(), cpu.end(), 0);
    float idle_time_delta = idle_time - previous_idle_time;
    float total_time_delta = total_time - previous_total_time;
    float utilization = 100.0 * (1.0 - idle_time_delta / total_time_delta);
    previous_idle_time = idle_time;
    previous_total_time = total_time;

    return utilization;
}

float get_memory_usage()
{
    vector<string> memory;
    string str;
    ifstream proc_mem("/proc/meminfo");
    for( string str; proc_mem >> str; memory.push_back(str));
    size_t memTotal;
    size_t memAvailable;
    float memUsed;
    stringstream sstream(memory[1]);
    stringstream ssstream(memory[7]);
    sstream >> memTotal;
    ssstream >> memAvailable;
    memUsed = ((float)(memTotal - memAvailable)) / 1000000;
    return memUsed;

}

string get_PID_ngnix()
{
    ifstream pid;
    pid.open( "/usr/local/nginx/logs/nginx.pid" );
    string str;

    if( !pid.is_open() )
	{
		return "N/A  ";
	}
	else
    {
        getline( pid, str );
        pid.close();
        return str;
    }
}

string get_ingest_server( string choice, string streamService  )
{
    string twitchServers[] = {  "rtmp://live-ams.twitch.tv/app/",
                                "rtmp://live-arn.twitch.tv/app/",
                                "rtmp://live-cdg.twitch.tv/app/",
                                "rtmp://live-dfw.twitch.tv/app/",
                                "rtmp://live-eze.twitch.tv/app/",
                                "rtmp://live-fra.twitch.tv/app/",
                                "rtmp://live-gig.twitch.tv/app/",
                                "rtmp://live-gru.twitch.tv/app/",
                                "rtmp://live-hkg.twitch.tv/app/",
                                "rtmp://live-iad.twitch.tv/app/",
                                "rtmp://live-jfk.twitch.tv/app/",
                                "rtmp://live-lax.twitch.tv/app/",
                                "rtmp://live-lhr.twitch.tv/app/",
                                "rtmp://live-mia.twitch.tv/app/",
                                "rtmp://live-ord.twitch.tv/app/",
                                "rtmp://live-prg.twitch.tv/app/",
                                "rtmp://live-scl.twitch.tv/app/",
                                "rtmp://live-sea.twitch.tv/app/",
                                "rtmp://live-sel.twitch.tv/app/",
                                "rtmp://live-sin.twitch.tv/app/",
                                "rtmp://live-sjc.twitch.tv/app/",
                                "rtmp://live-syd.twitch.tv/app/",
                                "rtmp://live-tpe.twitch.tv/app/",
                                "rtmp://live-tyo.twitch.tv/app/",
                                "rtmp://live-waw.twitch.tv/app/",
                                "rtmp://live.twitch.tv/app/",
                                "NULL" };


    if( streamService.compare( "Twitch" ) == 0 )
    {
        if( choice.compare("Amsterdamn") == 0 )
        {
            ingestServerName = "Amsterdamn";
            return twitchServers[0];
        }
        else if ( choice.compare( "Stockholm" ) == 0  )
        {
            ingestServerName = "Stockholm";
            return twitchServers[1];
        }
        else if ( choice.compare( "Paris" ) == 0  )
        {
            ingestServerName = "Paris";
            return twitchServers[2];
        }
        else if ( choice.compare( "Dallas" ) == 0  )
        {
            ingestServerName = "Dallas";
            return twitchServers[3];
        }
        else if ( choice.compare( "Argentina" ) == 0  )
        {
            ingestServerName = "Argentina";
            return twitchServers[4];
        }
        else if ( choice.compare( "Frankfurt" ) == 0  )
        {
            ingestServerName = "Frankfurt";
            return twitchServers[5];
        }
        else if ( choice.compare( "Rio de Janeiro" ) == 0  )
        {
            ingestServerName = "Rio de Janeiro";
            return twitchServers[6];
        }
        else if ( choice.compare( "Sao Paulo" ) == 0  )
        {
            ingestServerName = "Sao Paulo";
            return twitchServers[7];
        }
        else if ( choice.compare( "Hong Kong" ) == 0  )
        {
            ingestServerName = "Hong Kong";
            return twitchServers[8];
        }
        else if ( choice.compare( "Ashburn" ) == 0  )
        {
            ingestServerName = "Ashburn";
            return twitchServers[9];
        }
        else if ( choice.compare( "New York" ) == 0  )
        {
            ingestServerName = "New York";
            return twitchServers[10];
        }
        else if ( choice.compare( "Los Angeles" ) == 0  )
        {
            ingestServerName = "Los Angeles";
            return twitchServers[11];
        }
        else if ( choice.compare( "London" ) == 0  )
        {
            ingestServerName = "London";
            return twitchServers[12];
        }
        else if ( choice.compare( "Miami" ) == 0  )
        {
            ingestServerName = "Miami";
            return twitchServers[13];
        }
        else if ( choice.compare( "Chicago" ) == 0  )
        {
            ingestServerName = "Chicago";
            return twitchServers[14];
        }
        else if ( choice.compare( "Prague" ) == 0  )
        {
            ingestServerName = "Prague";
            return twitchServers[15];
        }
        else if ( choice.compare( "Chile" ) == 0  )
        {
            ingestServerName = "Chile";
            return twitchServers[16];
        }
        else if ( choice.compare( "Seattle" ) == 0  )
        {
            ingestServerName = "Seattle";
            return twitchServers[17];
        }
        else if ( choice.compare( "Seoul" ) == 0  )
        {
            ingestServerName = "Seoul";
            return twitchServers[18];
        }
        else if ( choice.compare( "Singapore" ) == 0  )
        {
            ingestServerName = "Singapore";
            return twitchServers[19];
        }
        else if ( choice.compare( "San Jose" ) == 0  )
        {
            ingestServerName = "San Jose";
            return twitchServers[20];
        }
        else if ( choice.compare( "Sydney" ) == 0  )
        {
            ingestServerName = "Sydney";
            return twitchServers[21];
        }
        else if ( choice.compare( "Taipei" ) == 0  )
        {
            ingestServerName = "Taipei";
            return twitchServers[22];
        }
        else if ( choice.compare( "Tokyo" ) == 0  )
        {
            ingestServerName = "Tokyo";
            return twitchServers[23];
        }
        else if ( choice.compare( "Warsaw" ) == 0  )
        {
            ingestServerName = "Warsaw";
            return twitchServers[24];
        }
        else if ( choice.compare( "San Francisco" ) == 0  )
        {
            ingestServerName = "San Francisco";
            return twitchServers[24];
        }
        else
        {
            ingestServerName = "NULL";
            return twitchServers[25];
        }
    }

   return "No Server Selected";
}

string get_streamKey( string str )
{
    char *tokens = strtok( (char*)str.c_str(),"/" );
    string temp1;


    while(tokens)
    {
        tokens = strtok(NULL,"/");
        if( tokens != NULL)
        {
            string temp(tokens);
            temp1 = temp;
        }
    }

    temp1.insert(temp1.end()-1,'\0' );

    return temp1;

}
