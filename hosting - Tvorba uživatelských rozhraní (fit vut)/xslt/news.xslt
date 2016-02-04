<?xml version="1.0" encoding="utf-8"?>
<!--
 * news.xslt:      Sprava hostingu (ITU 2012)
 *
 * Author(s):      Jan Havlik     <xhavli30@stud.fit.vutbr.cz>
 *                 Vaclav Horazny <xhoraz01@stud.fit.vutbr.cz>
 *                 Radek Sevcik   <xsevci44@stud.fit.vutbr.cz>
 *
 * Date:           Sun, 16 Dec 2012 13:36:03 +0100
 *
 * This file is part of itu12_hosting.
-->

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" encoding="iso-8859-2" indent="yes"/>

  <xsl:template match="/">
    <xsl:apply-templates select="//item"/>
  </xsl:template>

  <xsl:template match="item[last()]">
    <div class="zprava_nadpis">
      <xsl:value-of select="@title"/>
    </div>
    <div class="zprava_text">
      <xsl:value-of select="text()"/>
    </div>
    <div class="zprava_datum_cas">
      <xsl:value-of select="@date"/>
    </div>
  </xsl:template>

  <xsl:template match="*"/>

</xsl:stylesheet>
