//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file RinexNavData.cpp
 * Encapsulates RINEX Navigation data
 */

#include "StringUtils.hpp"

#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"

#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "GNSSconstants.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   using namespace gpstk::StringUtils;
   using namespace std;

   RinexNavData::RinexNavData()
         : time(gpstk::CommonTime::BEGINNING_OF_TIME), PRNID(-1),
           sf1XmitTime(0), toeWeek(0), codeflgs(0), accuracy(0),
           health(0), L2Pdata(0), IODC(0), IODE(0), af0(0), af1(0), af2(0),
           Tgd(0), Cuc(0), Cus(0), Crc(0), Crs(0), Cic(0), Cis(0), Toe(0),
           M0(0), dn(0), ecc(0), Ahalf(0), OMEGA0(0), i0(0), w(0), OMEGAdot(0),
           idot(0), fitint(4)
   {
      time.setTimeSystem(gpstk::TimeSystem::GPS);
   }

   RinexNavData::RinexNavData(const EngEphemeris& ee)
   {
      time = ee.getEpochTime();
      PRNID = ee.getPRNID();
      codeflgs = ee.getCodeFlags();
      accuracy = ee.getAccuracy();
      health = ee.getHealth();
      L2Pdata = ee.getL2Pdata();
      IODC = ee.getIODC();
      IODE = ee.getIODE();

      af0 = ee.getAf0();
      af1 = ee.getAf1();
      af2 = ee.getAf2();
      Tgd = ee.getTgd();

      Cuc = ee.getCuc();
      Cus = ee.getCus();
      Crc = ee.getCrc();
      Crs = ee.getCrs();
      Cic = ee.getCic();
      Cis = ee.getCis();

      Toe = ee.getToe();
      setXmitTime(ee.getFullWeek(), ee.getHOWTime(1));
      M0 = ee.getM0();
      dn = ee.getDn();
      ecc = ee.getEcc();
      Ahalf = ee.getAhalf();
      OMEGA0 = ee.getOmega0();
      i0 = ee.getI0();
      w = ee.getW();
      OMEGAdot = ee.getOmegaDot();
      idot = ee.getIDot();
      fitint = ee.getFitInterval();
   }

   void RinexNavData::reallyPutRecord(FFStream& ffs) const
      throw(std::exception, FFStreamError, StringException)
   {
      RinexNavStream& strm = dynamic_cast<RinexNavStream&>(ffs);

      strm << putPRNEpoch() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit1() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit2() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit3() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit4() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit5() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit6() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit7(strm.header.version) << endl;
      strm.lineNumber++;
   }

   void RinexNavData::reallyGetRecord(FFStream& ffs)
      throw(std::exception, FFStreamError, StringException)
   {
      RinexNavStream& strm = dynamic_cast<RinexNavStream&>(ffs);

         // If the header hasn't been read, read it...
      if(!strm.headerRead)
         strm >> strm.header;

      string line;

      strm.formattedGetLine(line, true);
      getPRNEpoch(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit1(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit2(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit3(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit4(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit5(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit6(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit7(line);
   }

   std::string RinexNavData::stableText() const
   {
      ostringstream s;
      s << "PRN: " << setw(2) << PRNID
        << " TOE: " << printTime(getToeTime(), "%02m/%02d/%04Y %02H:%02M:%02S")
        << " TOC: " << printTime(time, "%4F %10.3g")
        << " IODE: " << setw(4) << int(IODE)            // IODE should be int
        << " HOWtime: " << setw(6) << getHOWWS().sow;   // HOW should be double
      return s.str();
   }

   void RinexNavData::dump(ostream& s) const
   {
      s << "PRN: " << setw(2) << PRNID
        << " TOE: " << printTime(getToeTime(), "%02m/%02d/%04Y %02H:%02M:%02S")
        << " TOC: " << printTime(time, "%02m/%02d/%04Y %02H:%02M:%02S")
        << " IODE: " << setw(4) << int(IODE)            // IODE should be int
        << " HOWtime: " << setw(6) << getHOWWS().sow    // HOW should be double
        << endl;
   }

   RinexNavData::operator EngEphemeris() const throw()
   {
      EngEphemeris ee;

         // there's no TLM word in RinexNavData, so it's set to 0.
         // likewise, there's no AS alert or tracker.
         // Also, in Rinex, the accuracy is in meters, and setSF1 expects
         // the accuracy flag.  We'll give it zero and pass the accuracy
         // separately via the setAccuracy() method.
      CommonTime how1(getHOWTime()), how2(how1+6), how3(how2+6);
      GPSWeekSecond ws1(how1), ws2(how2), ws3(how3);
      ee.setSF1(0, ws1.sow, 0, ws1.week, codeflgs, 0, health,
                short(IODC), L2Pdata, Tgd, getTocWS().sow, af2, af1, af0, 0,
                PRNID);
      ee.setSF2(0, ws2.sow, 0, short(IODE), Crs, dn, M0, Cuc, ecc, Cus, Ahalf,
                Toe, (fitint > 4) ? 1 : 0);
      ee.setSF3(0, ws3.sow, 0, Cic, OMEGA0, Cis, i0, Crc, w, OMEGAdot,
                idot);
      ee.setFIC(false);
      ee.setAccuracy(accuracy);

      return ee;
   }

      // Convert this RinexNavData to a GPSEphemeris object.
      // for backward compatibility only - use Rinex3NavData
   RinexNavData::operator GPSEphemeris() const
   {
      GPSEphemeris gpse;
      try
      {
            // Overhead
         gpse.satID = SatID(PRNID, SatID::systemGPS);
         gpse.ctToe = time;

            // clock model
         gpse.af0 = af0;
         gpse.af1 = af1;
         gpse.af2 = af2;
   
            // Major orbit parameters
         gpse.M0 = M0;
         gpse.dn = dn;
         gpse.ecc = ecc;
         gpse.A = Ahalf * Ahalf;
         gpse.OMEGA0 = OMEGA0;
         gpse.i0 = i0;
         gpse.w = w;
         gpse.OMEGAdot = OMEGAdot;
         gpse.idot = idot;
            // modern nav msg
         gpse.dndot = 0.;
         gpse.Adot = 0.;
   
            // Harmonic perturbations
         gpse.Cuc = Cuc;
         gpse.Cus = Cus;
         gpse.Crc = Crc;
         gpse.Crs = Crs;
         gpse.Cic = Cic;
         gpse.Cis = Cis;
   
         gpse.dataLoadedFlag = true;

         gpse.ctToc = time;
         gpse.ctToc.setTimeSystem(TimeSystem::GPS);

            // now load the GPS-specific parts
         gpse.IODC = IODC;
         gpse.IODE = IODE;
         gpse.health = health;
         gpse.accuracyFlag = accuracy;
         gpse.Tgd = Tgd;

         gpse.HOWtime = getHOWWS().sow;
         gpse.transmitTime = getXmitTime();
         gpse.transmitTime.setTimeSystem(TimeSystem::GPS);

         gpse.codeflags = codeflgs;
         gpse.L2Pdata = L2Pdata;

            /// @note IODC must be set first...
         gpse.fitint = fitint;
         gpse.setFitIntervalFlag(int(fitint)); 
         gpse.adjustValidity();
      }
      catch(Exception& e)
      {
         GPSTK_RETHROW(e);
      }
      return gpse;
   }

   list<double> RinexNavData::toList() const
   {
      list<double> l;
      GPSWeekSecond howws(getHOWWS());

      l.push_back(PRNID);
      l.push_back(howws.sow);
      l.push_back(howws.week);
      l.push_back(codeflgs);
      l.push_back(accuracy);
      l.push_back(health);
      l.push_back(L2Pdata);
      l.push_back(IODC);
      l.push_back(IODE);
      l.push_back(getTocWS().sow);
      l.push_back(af0);
      l.push_back(af1);
      l.push_back(af2);
      l.push_back(Tgd);
      l.push_back(Cuc);
      l.push_back(Cus);
      l.push_back(Crc);
      l.push_back(Crs);
      l.push_back(Cic);
      l.push_back(Cis);
      l.push_back(Toe);
      l.push_back(M0);
      l.push_back(dn);
      l.push_back(ecc);
      l.push_back(Ahalf);
      l.push_back(OMEGA0);
      l.push_back(i0);
      l.push_back(w);
      l.push_back(OMEGAdot);
      l.push_back(idot);
      l.push_back(fitint);

      return l;
   }

   string RinexNavData::putPRNEpoch(void) const
      throw(StringException)
   {
      string line;
      line += rightJustify(asString(PRNID), 2);
      line += writeTime(time);
      line += string(1, ' ');
      line += doub2for(af0, 18, 2);
      line += string(1, ' ');
      line += doub2for(af1, 18, 2);
      line += string(1, ' ');
      line += doub2for(af2, 18, 2);
      return line;
   }

   string RinexNavData::writeTime(const CommonTime& dt) const
      throw(StringException)
   {
      return printTime(dt, " %02y %2m %2d %2H %2M%5.1f");
   }

   string RinexNavData::putBroadcastOrbit1(void) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(IODE, 18, 2);
      line += string(1, ' ');
      line += doub2for(Crs, 18, 2);
      line += string(1, ' ');
      line += doub2for(dn, 18, 2);
      line += string(1, ' ');
      line += doub2for(M0, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit2(void) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(Cuc, 18, 2);
      line += string(1, ' ');
      line += doub2for(ecc, 18, 2);
      line += string(1, ' ');
      line += doub2for(Cus, 18, 2);
      line += string(1, ' ');
      line += doub2for(Ahalf, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit3(void) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(Toe, 18, 2);
      line += string(1, ' ');
      line += doub2for(Cic, 18, 2);
      line += string(1, ' ');
      line += doub2for(OMEGA0, 18, 2);
      line += string(1, ' ');
      line += doub2for(Cis, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit4(void) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(i0, 18, 2);
      line += string(1, ' ');
      line += doub2for(Crc, 18, 2);
      line += string(1, ' ');
      line += doub2for(w, 18, 2);
      line += string(1, ' ');
      line += doub2for(OMEGAdot, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit5(void) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(idot, 18, 2);
      line += string(1, ' ');
      line += doub2for((double)codeflgs, 18, 2);
      line += string(1, ' ');
      line += doub2for(toeWeek, 18, 2);
      line += string(1, ' ');
      line += doub2for((double)L2Pdata, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit6(void) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(accuracy, 18, 2);
      line += string(1, ' ');
      line += doub2for((double)health, 18, 2);
      line += string(1, ' ');
      line += doub2for(Tgd, 18, 2);
      line += string(1, ' ');
      line += doub2for(IODC, 18, 2);
      return line;
   }

   string RinexNavData::putBroadcastOrbit7(const double ver) const
      throw(StringException)
   {
      string line;
      line += string(3, ' ');
      line += string(1, ' ');
      line += doub2for(sf1XmitTime, 18, 2);

      if (ver >= 2.1)
      {
         line += string(1, ' ');
         line += doub2for(fitint, 18, 2);
      }
      return line;
   }

   void RinexNavData::getPRNEpoch(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
            // check for spaces in the right spots...
         for (int i = 2; i <= 17; i += 3)
            if (currentLine[i] != ' ')
               throw(FFStreamError("Badly formatted line"));

         PRNID = asInt(currentLine.substr(0,2));

         short yr = asInt(currentLine.substr(2,3));
         short mo = asInt(currentLine.substr(5,3));
         short day = asInt(currentLine.substr(8,3));
         short hr = asInt(currentLine.substr(11,3));
         short min = asInt(currentLine.substr(14,3));
         double sec = asDouble(currentLine.substr(17,5));

            // years 80-99 represent 1980-1999
         const int rolloverYear = 80;
         if (yr < rolloverYear)
            yr += 100;
         yr += 1900;

            // Real Rinex has epochs 'yy mm dd hr 59 60.0'
            // surprisingly often....
         double ds=0;
         if(sec >= 60.)
         {
            ds=sec;
            sec=0.0;
         }
         time = CivilTime(yr,mo,day,hr,min,sec,gpstk::TimeSystem::GPS).convertToCommonTime();
         if(ds != 0) time += ds;

         af0 = gpstk::StringUtils::for2doub(currentLine.substr(22,19));
         af1 = gpstk::StringUtils::for2doub(currentLine.substr(41,19));
         af2 = gpstk::StringUtils::for2doub(currentLine.substr(60,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit1(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         IODE = gpstk::StringUtils::for2doub(currentLine.substr(3,19));
         Crs = gpstk::StringUtils::for2doub(currentLine.substr(22,19));
         dn = gpstk::StringUtils::for2doub(currentLine.substr(41,19));
         M0 = gpstk::StringUtils::for2doub(currentLine.substr(60,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit2(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         Cuc = gpstk::StringUtils::for2doub(currentLine.substr(3,19));
         ecc = gpstk::StringUtils::for2doub(currentLine.substr(22,19));
         Cus = gpstk::StringUtils::for2doub(currentLine.substr(41,19));
         Ahalf = gpstk::StringUtils::for2doub(currentLine.substr(60,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit3(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         Toe = gpstk::StringUtils::for2doub(currentLine.substr(3,19));
         Cic = gpstk::StringUtils::for2doub(currentLine.substr(22,19));
         OMEGA0 = gpstk::StringUtils::for2doub(currentLine.substr(41,19));
         Cis = gpstk::StringUtils::for2doub(currentLine.substr(60,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit4(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         i0 = gpstk::StringUtils::for2doub(currentLine.substr(3,19));
         Crc = gpstk::StringUtils::for2doub(currentLine.substr(22,19));
         w = gpstk::StringUtils::for2doub(currentLine.substr(41,19));
         OMEGAdot = gpstk::StringUtils::for2doub(currentLine.substr(60,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit5(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         double codeL2, L2P, toe_wn;

         idot = gpstk::StringUtils::for2doub(currentLine.substr(3,19));
         codeL2 = gpstk::StringUtils::for2doub(currentLine.substr(22,19));
         toe_wn = gpstk::StringUtils::for2doub(currentLine.substr(41,19));
         L2P = gpstk::StringUtils::for2doub(currentLine.substr(60,19));

         codeflgs = (short) codeL2;
         L2Pdata = (short) L2P;
         toeWeek = (short) toe_wn;
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit6(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         double SV_health;

         accuracy = gpstk::StringUtils::for2doub(currentLine.substr(3,19));
         SV_health = gpstk::StringUtils::for2doub(currentLine.substr(22,19));
         Tgd = gpstk::StringUtils::for2doub(currentLine.substr(41,19));
         IODC = gpstk::StringUtils::for2doub(currentLine.substr(60,19));


         health = (short) SV_health;
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void RinexNavData::getBroadcastOrbit7(const string& currentLine)
      throw(StringException, FFStreamError)
   {
      try
      {
         double HOW_sec;

         HOW_sec = gpstk::StringUtils::for2doub(currentLine.substr(3,19));
            // leave it alone so round-trips are possible
            // (even though we're storing a double as a long, which
            //could lead to failures in round-trip testing, though if
            //that happens your transmit time is messed).
            //setXmitTime(HOW_sec);
         sf1XmitTime = HOW_sec;
         fitint = gpstk::StringUtils::for2doub(currentLine.substr(22,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " +
                           string(e.what()));
         GPSTK_THROW(err);
      }
   }


   GPSWeekSecond RinexNavData::getXmitWS() const
   {
      GPSWeekSecond rv;
         // sf1XmitTime may not actually be a proper subframe 1
         // transmit time.  It may be a HOW time or something like
         // that.
      if (sf1XmitTime < 0)
      {
            // If the transmit time is negative, assume that it
            // corresponds to the Toe week, according to the footnote
            // attached to Table A4 in the 2.11 standard.
         long properXmit = fixSF1xmitSOW(sf1XmitTime+FULLWEEK);
         rv = GPSWeekSecond(toeWeek-1, properXmit, TimeSystem::GPS);
      }
      else
      {
            // If the transmit time is >= 0, make sure that we have
            // the right week using a trusty old half-week test.
         long properXmit = fixSF1xmitSOW(sf1XmitTime);
         double diff = Toe - properXmit;
         if (diff < -HALFWEEK)
            rv = GPSWeekSecond(toeWeek-1, properXmit, TimeSystem::GPS);
         else if (diff > HALFWEEK)
            rv = GPSWeekSecond(toeWeek+1, properXmit, TimeSystem::GPS);
         else
            rv = GPSWeekSecond(toeWeek, properXmit, TimeSystem::GPS);
      }
      return rv;
   }


   RinexNavData& RinexNavData::setXmitWeek(unsigned short fullweek)
   {
      if (sf1XmitTime < 0)
      {
            // If the transmit time is negative, assume that the
            // transmit week corresponds to the Toe week, according to
            // the footnote attached to Table A4 in the 2.11 standard.
         toeWeek = fullweek;
      }
      else
      {
            // If the transmit time is >= 0, make sure that we have
            // the right week using a trusty old half-week test.
         double diff = Toe - sf1XmitTime;
         if (diff < -HALFWEEK)
         {
            toeWeek = fullweek+1;
               // adjust  transmit time to be relative to the week.
               // week is in broadcast orbit 5
               // transmission time is in broadcast orbit 7
               //   see footnote in RINEX 2.11 document
            sf1XmitTime -= FULLWEEK;
         }
         else if (diff > HALFWEEK)
         {
            toeWeek = fullweek-1;
               // see comments above
            sf1XmitTime += FULLWEEK;
         }
         else
            toeWeek = fullweek;
      }
      return *this;
   }


   RinexNavData& RinexNavData::setXmitTime(unsigned short fullweek,
                                           unsigned long sow)
   {
      setXmitTime(sow);
      setXmitWeek(fullweek);
      return *this;
   }


}  // end of namespace
