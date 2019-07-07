/********************************************************
 * Description : http utility
 * Data        : 2018-04-07 22:39:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#include <cstring>
#include <map>
#include "http_utility.h"

namespace BoostWeb { // namespace BoostWeb begin

struct compare_string_ignore_case_t
{
    bool operator() (const std::string & lhs, const std::string & rhs) const
    {
#ifdef _MSC_VER
        return (stricmp(lhs.c_str(), rhs.c_str()) < 0);
#else
        return (strcasecmp(lhs.c_str(), rhs.c_str()) < 0);
#endif // _MSC_VER
    }
};

struct MimeType
{
    MimeType();
    const std::string & operator () (const std::string & file_suffix) const;

    std::map<std::string, std::string, compare_string_ignore_case_t>    m_mime_type_map;
    const std::string                                                   m_default_mime_type;
};

MimeType::MimeType()
    : m_mime_type_map()
    , m_default_mime_type("application/octet-stream")
{
    m_mime_type_map["."] = "application/x-";
    m_mime_type_map[".001"] = "application/x-001";
    m_mime_type_map[".301"] = "application/x-301";
    m_mime_type_map[".323"] = "text/h323";
    m_mime_type_map[".3gp"] = "video/3gpp";
    m_mime_type_map[".906"] = "application/x-906";
    m_mime_type_map[".907"] = "drawing/907";
    m_mime_type_map[".ivf"] = "video/x-ivf";
    m_mime_type_map[".a11"] = "application/x-a11";
    m_mime_type_map[".acp"] = "audio/x-mei-aac";
    m_mime_type_map[".ai"] = "application/postscript";
    m_mime_type_map[".aif"] = "audio/aiff";
    m_mime_type_map[".aifc"] = "audio/aiff";
    m_mime_type_map[".aiff"] = "audio/aiff";
    m_mime_type_map[".anv"] = "application/x-anv";
    m_mime_type_map[".asa"] = "text/asa";
    m_mime_type_map[".asf"] = "video/x-ms-asf";
    m_mime_type_map[".asp"] = "text/asp";
    m_mime_type_map[".asx"] = "video/x-ms-asf";
    m_mime_type_map[".au"] = "audio/basic";
    m_mime_type_map[".avi"] = "video/x-msvideo";
    m_mime_type_map[".awf"] = "application/vnd.adobe.workflow";
    m_mime_type_map[".biz"] = "text/xml";
    m_mime_type_map[".bmp"] = "application/x-MS-bmp";
    m_mime_type_map[".bot"] = "application/x-bot";
    m_mime_type_map[".c4t"] = "application/x-c4t";
    m_mime_type_map[".c90"] = "application/x-c90";
    m_mime_type_map[".cab"] = "application/x-shockwave-flash";
    m_mime_type_map[".cal"] = "application/x-cals";
    m_mime_type_map[".cat"] = "application/vnd.ms-pki.seccat";
    m_mime_type_map[".cdf"] = "application/x-netcdf";
    m_mime_type_map[".cdr"] = "application/x-cdr";
    m_mime_type_map[".cel"] = "application/x-cel";
    m_mime_type_map[".cer"] = "application/x-x509-ca-cert";
    m_mime_type_map[".cg4"] = "application/x-g4";
    m_mime_type_map[".cgm"] = "application/x-cgm";
    m_mime_type_map[".chm"] = "application/mshelp";
    m_mime_type_map[".cit"] = "application/x-cit";
    m_mime_type_map[".class"] = "java/*";
    m_mime_type_map[".cml"] = "text/xml";
    m_mime_type_map[".cmp"] = "application/x-cmp";
    m_mime_type_map[".cmx"] = "application/x-cmx";
    m_mime_type_map[".cot"] = "application/x-cot";
    m_mime_type_map[".crl"] = "application/pkix-crl";
    m_mime_type_map[".crt"] = "application/x-x509-ca-cert";
    m_mime_type_map[".csi"] = "application/x-csi";
    m_mime_type_map[".css"] = "text/css";
    m_mime_type_map[".cut"] = "application/x-cut";
    m_mime_type_map[".dbf"] = "application/x-dbf";
    m_mime_type_map[".dbm"] = "application/x-dbm";
    m_mime_type_map[".dbx"] = "application/x-dbx";
    m_mime_type_map[".dcd"] = "text/xml";
    m_mime_type_map[".dcx"] = "application/x-dcx";
    m_mime_type_map[".der"] = "application/x-x509-ca-cert";
    m_mime_type_map[".dgn"] = "application/x-dgn";
    m_mime_type_map[".dib"] = "application/x-dib";
    m_mime_type_map[".dll"] = "application/x-msdownload";
    m_mime_type_map[".doc"] = "application/msword";
    m_mime_type_map[".dot"] = "application/msword";
    m_mime_type_map[".drw"] = "application/x-drw";
    m_mime_type_map[".dtd"] = "text/xml";
    m_mime_type_map[".dwf"] = "application/x-dwf";
    m_mime_type_map[".dwg"] = "application/x-dwg";
    m_mime_type_map[".dxb"] = "application/x-dxb";
    m_mime_type_map[".dxf"] = "application/x-dxf";
    m_mime_type_map[".edn"] = "application/vnd.adobe.edn";
    m_mime_type_map[".emf"] = "application/x-emf";
    m_mime_type_map[".eml"] = "message/rfc822";
    m_mime_type_map[".ent"] = "text/xml";
    m_mime_type_map[".epi"] = "application/x-epi";
    m_mime_type_map[".eps"] = "application/postscript";
    m_mime_type_map[".etd"] = "application/x-ebx";
    m_mime_type_map[".exe"] = "application/x-msdownload";
    m_mime_type_map[".fax"] = "image/fax";
    m_mime_type_map[".fdf"] = "application/vnd.fdf";
    m_mime_type_map[".fif"] = "application/fractals";
    m_mime_type_map[".fo"] = "text/xml";
    m_mime_type_map[".frm"] = "application/x-frm";
    m_mime_type_map[".g4"] = "application/x-g4";
    m_mime_type_map[".gbr"] = "application/x-gbr";
    m_mime_type_map[".gif"] = "image/gif";
    m_mime_type_map[".gl2"] = "application/x-gl2";
    m_mime_type_map[".gp4"] = "application/x-gp4";
    m_mime_type_map[".gz"] = "application/x-gzip";
    m_mime_type_map[".hgl"] = "application/x-hgl";
    m_mime_type_map[".hlp"] = "application/mshelp";
    m_mime_type_map[".hmr"] = "application/x-hmr";
    m_mime_type_map[".hpg"] = "application/x-hpgl";
    m_mime_type_map[".hpl"] = "application/x-hpl";
    m_mime_type_map[".hqx"] = "application/mac-binhex40";
    m_mime_type_map[".hrf"] = "application/x-hrf";
    m_mime_type_map[".hta"] = "application/hta";
    m_mime_type_map[".htc"] = "text/x-component";
    m_mime_type_map[".htm"] = "text/html";
    m_mime_type_map[".html"] = "text/html";
    m_mime_type_map[".hts"] = "text/html";
    m_mime_type_map[".htt"] = "text/webviewhtml";
    m_mime_type_map[".htx"] = "text/html";
    m_mime_type_map[".icb"] = "application/x-icb";
    m_mime_type_map[".ico"] = "application/x-ico";
    m_mime_type_map[".iff"] = "application/x-iff";
    m_mime_type_map[".ig4"] = "application/x-g4";
    m_mime_type_map[".igs"] = "application/x-igs";
    m_mime_type_map[".iii"] = "application/x-iphone";
    m_mime_type_map[".img"] = "application/x-img";
    m_mime_type_map[".ins"] = "application/x-internet-signup";
    m_mime_type_map[".isp"] = "application/x-internet-signup";
    m_mime_type_map[".jar"] = "application/java-archive";
    m_mime_type_map[".java"] = "java/*";
    m_mime_type_map[".jfif"] = "image/jpeg";
    m_mime_type_map[".jpe"] = "image/jpeg";
    m_mime_type_map[".jpeg"] = "image/jpeg";
    m_mime_type_map[".jpg"] = "image/jpeg";
    m_mime_type_map[".jpz"] = "image/jpeg";
    m_mime_type_map[".js"] = "text/javascript";
    m_mime_type_map[".json"] = "application/json";
    m_mime_type_map[".jsp"] = "text/html";
    m_mime_type_map[".la1"] = "audio/x-liquid-file";
    m_mime_type_map[".lar"] = "application/x-laplayer-reg";
    m_mime_type_map[".latex"] = "application/x-latex";
    m_mime_type_map[".lavs"] = "audio/x-liquid-secure";
    m_mime_type_map[".lbm"] = "application/x-lbm";
    m_mime_type_map[".lmsff"] = "audio/x-la-lms";
    m_mime_type_map[".log"] = "text/plain";
    m_mime_type_map[".ls"] = "application/x-javascript";
    m_mime_type_map[".ltr"] = "application/x-ltr";
    m_mime_type_map[".m1v"] = "video/x-mpeg";
    m_mime_type_map[".m2v"] = "video/x-mpeg";
    m_mime_type_map[".m3u"] = "audio/mpegurl";
    m_mime_type_map[".m4e"] = "video/mpeg4";
    m_mime_type_map[".mac"] = "application/x-mac";
    m_mime_type_map[".man"] = "application/x-troff-man";
    m_mime_type_map[".math"] = "text/xml";
    m_mime_type_map[".mdb"] = "application/x-mdb";
    m_mime_type_map[".mfp"] = "application/x-shockwave-flash";
    m_mime_type_map[".mht"] = "message/rfc822";
    m_mime_type_map[".mhtml"] = "message/rfc822";
    m_mime_type_map[".mi"] = "application/x-mi";
    m_mime_type_map[".mid"] = "audio/midi";
    m_mime_type_map[".midi"] = "audio/midi";
    m_mime_type_map[".mil"] = "application/x-mil";
    m_mime_type_map[".mml"] = "text/xml";
    m_mime_type_map[".mnd"] = "audio/x-musicnet-download";
    m_mime_type_map[".mns"] = "audio/x-musicnet-stream";
    m_mime_type_map[".mocha"] = "application/x-javascript";
    m_mime_type_map[".mov"] = "video/quicktime";
    m_mime_type_map[".movie"] = "video/x-sgi-movie";
    m_mime_type_map[".mp1"] = "audio/mp1";
    m_mime_type_map[".mp2"] = "audio/x-mpeg";
    m_mime_type_map[".mp2v"] = "video/mpeg";
    m_mime_type_map[".mp3"] = "audio/x-mpeg";
    m_mime_type_map[".mp4"] = "video/mp4";
    m_mime_type_map[".mpa"] = "video/x-mpg";
    m_mime_type_map[".mpd"] = "application/vnd.ms-project";
    m_mime_type_map[".mpe"] = "video/mpeg";
    m_mime_type_map[".mpeg"] = "video/mpeg";
    m_mime_type_map[".mpg"] = "video/mpeg";
    m_mime_type_map[".mpg4"] = "video/mp4";
    m_mime_type_map[".mpga"] = "video/mpeg";
    m_mime_type_map[".mpp"] = "application/vnd.ms-project";
    m_mime_type_map[".mps"] = "video/x-mpeg";
    m_mime_type_map[".mpt"] = "application/vnd.ms-project";
    m_mime_type_map[".mpv"] = "video/mpg";
    m_mime_type_map[".mpv2"] = "video/mpeg";
    m_mime_type_map[".mpw"] = "application/vnd.ms-project";
    m_mime_type_map[".mpx"] = "application/vnd.ms-project";
    m_mime_type_map[".mtx"] = "text/xml";
    m_mime_type_map[".mxp"] = "application/x-mmxp";
    m_mime_type_map[".net"] = "image/pnetvue";
    m_mime_type_map[".nrf"] = "application/x-nrf";
    m_mime_type_map[".nws"] = "message/rfc822";
    m_mime_type_map[".odc"] = "text/x-ms-odc";
    m_mime_type_map[".out"] = "application/x-out";
    m_mime_type_map[".p10"] = "application/pkcs10";
    m_mime_type_map[".p12"] = "application/x-pkcs12";
    m_mime_type_map[".p7b"] = "application/x-pkcs7-certificates";
    m_mime_type_map[".p7c"] = "application/pkcs7-mime";
    m_mime_type_map[".p7m"] = "application/pkcs7-mime";
    m_mime_type_map[".p7r"] = "application/x-pkcs7-certreqresp";
    m_mime_type_map[".p7s"] = "application/pkcs7-signature";
    m_mime_type_map[".pc5"] = "application/x-pc5";
    m_mime_type_map[".pci"] = "application/x-pci";
    m_mime_type_map[".pcl"] = "application/x-pcl";
    m_mime_type_map[".pcx"] = "application/x-pcx";
    m_mime_type_map[".pdf"] = "application/pdf";
    m_mime_type_map[".pdx"] = "application/vnd.adobe.pdx";
    m_mime_type_map[".pfx"] = "application/x-pkcs12";
    m_mime_type_map[".pgl"] = "application/x-pgl";
    m_mime_type_map[".pic"] = "application/x-pic";
    m_mime_type_map[".pko"] = "application/vnd.ms-pki.pko";
    m_mime_type_map[".pl"] = "application/x-perl";
    m_mime_type_map[".plg"] = "text/html";
    m_mime_type_map[".pls"] = "audio/scpls";
    m_mime_type_map[".plt"] = "application/x-plt";
    m_mime_type_map[".png"] = "image/png";
    m_mime_type_map[".pot"] = "application/mspowerpoint";
    m_mime_type_map[".ppa"] = "application/vnd.ms-powerpoint";
    m_mime_type_map[".ppm"] = "application/x-ppm";
    m_mime_type_map[".pps"] = "application/mspowerpoint";
    m_mime_type_map[".ppt"] = "application/mspowerpoint";
    m_mime_type_map[".ppz"] = "application/mspowerpoint";
    m_mime_type_map[".pr"] = "application/x-pr";
    m_mime_type_map[".prf"] = "application/pics-rules";
    m_mime_type_map[".prn"] = "application/x-prn";
    m_mime_type_map[".prt"] = "application/x-prt";
    m_mime_type_map[".ps"] = "application/postscript";
    m_mime_type_map[".ptn"] = "application/x-ptn";
    m_mime_type_map[".pwz"] = "application/vnd.ms-powerpoint";
    m_mime_type_map[".r3t"] = "text/vnd.rn-realtext3d";
    m_mime_type_map[".ra"] = "audio/vnd.rn-realaudio";
    m_mime_type_map[".ram"] = "audio/x-pn-realaudio";
    m_mime_type_map[".rar"] = "application/x-rar-compressed";
    m_mime_type_map[".ras"] = "application/x-ras";
    m_mime_type_map[".rat"] = "application/rat-file";
    m_mime_type_map[".rdf"] = "text/xml";
    m_mime_type_map[".rec"] = "application/vnd.rn-recording";
    m_mime_type_map[".red"] = "application/x-red";
    m_mime_type_map[".rgb"] = "application/x-rgb";
    m_mime_type_map[".rjs"] = "application/vnd.rn-realsystem-rjs";
    m_mime_type_map[".rjt"] = "application/vnd.rn-realsystem-rjt";
    m_mime_type_map[".rlc"] = "application/x-rlc";
    m_mime_type_map[".rle"] = "application/x-rle";
    m_mime_type_map[".rm"] = "audio/x-pn-realaudio";
    m_mime_type_map[".rmf"] = "application/vnd.adobe.rmf";
    m_mime_type_map[".rmi"] = "audio/mid";
    m_mime_type_map[".rmj"] = "application/vnd.rn-realsystem-rmj";
    m_mime_type_map[".rmm"] = "audio/x-pn-realaudio";
    m_mime_type_map[".rmp"] = "application/vnd.rn-rn_music_package";
    m_mime_type_map[".rms"] = "application/vnd.rn-realmedia-secure";
    m_mime_type_map[".rmvb"] = "audio/x-pn-realaudio";
    m_mime_type_map[".rmx"] = "application/vnd.rn-realsystem-rmx";
    m_mime_type_map[".rnx"] = "application/vnd.rn-realplayer";
    m_mime_type_map[".rp"] = "image/vnd.rn-realpix";
    m_mime_type_map[".rpm"] = "audio/x-pn-realaudio-plugin";
    m_mime_type_map[".rsml"] = "application/vnd.rn-rsml";
    m_mime_type_map[".rt"] = "text/vnd.rn-realtext";
    m_mime_type_map[".rtf"] = "application/rtf";
    m_mime_type_map[".rv"] = "video/vnd.rn-realvideo";
    m_mime_type_map[".sam"] = "application/x-sam";
    m_mime_type_map[".sat"] = "application/x-sat";
    m_mime_type_map[".sdp"] = "application/sdp";
    m_mime_type_map[".sdw"] = "application/x-sdw";
    m_mime_type_map[".shtml"] = "text/html";
    m_mime_type_map[".sit"] = "application/x-stuffit";
    m_mime_type_map[".slb"] = "application/x-slb";
    m_mime_type_map[".sld"] = "application/x-sld";
    m_mime_type_map[".slk"] = "drawing/x-slk";
    m_mime_type_map[".smi"] = "application/smil";
    m_mime_type_map[".smil"] = "application/smil";
    m_mime_type_map[".smk"] = "application/x-smk";
    m_mime_type_map[".snd"] = "audio/basic";
    m_mime_type_map[".sol"] = "text/plain";
    m_mime_type_map[".sor"] = "text/plain";
    m_mime_type_map[".spc"] = "application/x-pkcs7-certificates";
    m_mime_type_map[".spl"] = "application/futuresplash";
    m_mime_type_map[".spp"] = "text/xml";
    m_mime_type_map[".ssm"] = "application/streamingmedia";
    m_mime_type_map[".sst"] = "application/vnd.ms-pki.certstore";
    m_mime_type_map[".stl"] = "application/vnd.ms-pki.stl";
    m_mime_type_map[".stm"] = "text/html";
    m_mime_type_map[".sty"] = "application/x-sty";
    m_mime_type_map[".svg"] = "text/xml";
    m_mime_type_map[".swf"] = "application/x-shockwave-flash";
    m_mime_type_map[".swfl"] = "application/x-shockwave-flash";
    m_mime_type_map[".tar"] = "application/x-tar";
    m_mime_type_map[".tdf"] = "application/x-tdf";
    m_mime_type_map[".tg4"] = "application/x-tg4";
    m_mime_type_map[".tga"] = "application/x-tga";
    m_mime_type_map[".tgz"] = "application/x-tar";
    m_mime_type_map[".tif"] = "application/x-tif";
    m_mime_type_map[".tiff"] = "image/tiff";
    m_mime_type_map[".tld"] = "text/xml";
    m_mime_type_map[".top"] = "drawing/x-top";
    m_mime_type_map[".torrent"] = "application/x-bittorrent";
    m_mime_type_map[".tsd"] = "text/xml";
    m_mime_type_map[".txt"] = "text/plain";
    m_mime_type_map[".uin"] = "application/x-icq";
    m_mime_type_map[".uls"] = "text/iuls";
    m_mime_type_map[".vcf"] = "text/x-vcard";
    m_mime_type_map[".vda"] = "application/x-vda";
    m_mime_type_map[".vdx"] = "application/vnd.visio";
    m_mime_type_map[".vml"] = "text/xml";
    m_mime_type_map[".vpg"] = "application/x-vpeg005";
    m_mime_type_map[".vsd"] = "application/x-vsd";
    m_mime_type_map[".vss"] = "application/vnd.visio";
    m_mime_type_map[".vst"] = "application/x-vst";
    m_mime_type_map[".vsw"] = "application/vnd.visio";
    m_mime_type_map[".vsx"] = "application/vnd.visio";
    m_mime_type_map[".vtx"] = "application/vnd.visio";
    m_mime_type_map[".vxml"] = "text/xml";
    m_mime_type_map[".wav"] = "audio/wav";
    m_mime_type_map[".wax"] = "audio/x-ms-wax";
    m_mime_type_map[".wb1"] = "application/x-wb1";
    m_mime_type_map[".wb2"] = "application/x-wb2";
    m_mime_type_map[".wb3"] = "application/x-wb3";
    m_mime_type_map[".wbmp"] = "image/vnd.wap.wbmp";
    m_mime_type_map[".wiz"] = "application/msword";
    m_mime_type_map[".wk3"] = "application/x-wk3";
    m_mime_type_map[".wk4"] = "application/x-wk4";
    m_mime_type_map[".wkq"] = "application/x-wkq";
    m_mime_type_map[".wks"] = "application/x-wks";
    m_mime_type_map[".wm"] = "video/x-ms-wm";
    m_mime_type_map[".wma"] = "audio/x-ms-wma";
    m_mime_type_map[".wmd"] = "application/x-ms-wmd";
    m_mime_type_map[".wmf"] = "application/x-wmf";
    m_mime_type_map[".wml"] = "text/vnd.wap.wml";
    m_mime_type_map[".wmv"] = "audio/x-ms-wmv";
    m_mime_type_map[".wmx"] = "video/x-ms-wmx";
    m_mime_type_map[".wmz"] = "application/x-ms-wmz";
    m_mime_type_map[".wp6"] = "application/x-wp6";
    m_mime_type_map[".wpd"] = "application/x-wpd";
    m_mime_type_map[".wpg"] = "application/x-wpg";
    m_mime_type_map[".wpl"] = "application/vnd.ms-wpl";
    m_mime_type_map[".wq1"] = "application/x-wq1";
    m_mime_type_map[".wr1"] = "application/x-wr1";
    m_mime_type_map[".wri"] = "application/x-wri";
    m_mime_type_map[".wrk"] = "application/x-wrk";
    m_mime_type_map[".ws"] = "application/x-ws";
    m_mime_type_map[".ws2"] = "application/x-ws";
    m_mime_type_map[".wsc"] = "text/scriptlet";
    m_mime_type_map[".wsdl"] = "text/xml";
    m_mime_type_map[".wvx"] = "video/x-ms-wvx";
    m_mime_type_map[".x_b"] = "application/x-x_b";
    m_mime_type_map[".x_t"] = "application/x-x_t";
    m_mime_type_map[".xdp"] = "application/vnd.adobe.xdp";
    m_mime_type_map[".xdr"] = "text/xml";
    m_mime_type_map[".xfd"] = "application/vnd.adobe.xfd";
    m_mime_type_map[".xfdf"] = "application/vnd.adobe.xfdf";
    m_mime_type_map[".xht"] = "application/xhtml+xml";
    m_mime_type_map[".xhtm"] = "application/xhtml+xml";
    m_mime_type_map[".xhtml"] = "application/xhtml+xml";
    m_mime_type_map[".xla"] = "application/msexcel";
    m_mime_type_map[".xls"] = "application/msexcel";
    m_mime_type_map[".xlw"] = "application/x-xlw";
    m_mime_type_map[".xml"] = "text/xml";
    m_mime_type_map[".xpl"] = "audio/scpls";
    m_mime_type_map[".xq"] = "text/xml";
    m_mime_type_map[".xql"] = "text/xml";
    m_mime_type_map[".xquery"] = "text/xml";
    m_mime_type_map[".xsd"] = "text/xml";
    m_mime_type_map[".xsl"] = "text/xml";
    m_mime_type_map[".xslt"] = "text/xml";
    m_mime_type_map[".xwd"] = "application/x-xwd";
    m_mime_type_map[".zip"] = "application/zip";
}

const std::string & MimeType::operator () (const std::string & file_suffix) const
{
    std::map<std::string, std::string, compare_string_ignore_case_t>::const_iterator iter = m_mime_type_map.find(file_suffix);
    if (m_mime_type_map.end() != iter)
    {
        return (iter->second);
    }
    else
    {
        return (m_default_mime_type);
    }
};

const std::string & get_mime_type(const std::string & filename)
{
    const static MimeType mime_type;
    std::string::size_type pos = filename.find_last_of("./");
    if (std::string::npos != pos && '.' == filename[pos])
    {
        return (mime_type(filename.substr(pos)));
    }
    else
    {
        return (mime_type(""));
    }
}

std::string path_catenate(const std::string & base, const std::string & path)
{
    return (base + path);
}

} // namespace BoostWeb end
