<?xml version="1.0" encoding="utf-8"?>
<!--
 * web.xslt:       Sprava hostingu (ITU 2012)
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
    <form>
      <div class="webhosting">
        <xsl:apply-templates select="//record[@login = $login]"/>
      </div>
    </form>
  </xsl:template>

  <xsl:template match="record">
    <div class="domena">domena :</div>
    <xsl:element name="input">
        <xsl:attribute name="type">text</xsl:attribute>
        <xsl:attribute name="class">dom</xsl:attribute>
        <xsl:attribute name="value">
        <xsl:value-of select="@name" />
        </xsl:attribute>
    </xsl:element>

    <input class="edit" type="submit" value="" title="Ulozit" />
    <div class="cl"></div>
  </xsl:template>

  <xsl:template match="*"/>

</xsl:stylesheet>
