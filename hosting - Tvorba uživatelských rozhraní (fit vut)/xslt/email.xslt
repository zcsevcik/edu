<?xml version="1.0" encoding="utf-8"?>
<!--
 * email.xslt:     Sprava hostingu (ITU 2012)
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
    <form><div class="email">
      <xsl:apply-templates select="//domain[@login = $login]"/>
    </div></form>
  </xsl:template>

  <xsl:template match="//domain">
        <div class="domena">domena:</div>

        <xsl:element name="input">
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="class">dom</xsl:attribute>
          <xsl:attribute name="value">
            <xsl:value-of select="@name" />
          </xsl:attribute>
        </xsl:element>

        <input class="edit" type="submit" value="" title="Ulozit" />
        <div class="cl"></div>

        <div class="text">kapacita: <xsl:value-of select="@capacity" />MB</div>

        <input class="dokoupit" type="submit" value="prikoupit" title="Prikoupit" />
        <div class="cl"></div>
        <ul>
          <xsl:apply-templates select="user"/>
        </ul>
  </xsl:template>

  <xsl:template match="user">
    <li>
        <div class="e_tname">jmeno:</div>

        <xsl:element name="input">
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="class">e_name</xsl:attribute>
          <xsl:attribute name="value">
            <xsl:value-of select="@name" />
          </xsl:attribute>
        </xsl:element>

        <input class="editemail" type="submit" value="" title="Ulozit" />
        <div class="e_tquota">quota:</div>

        <xsl:element name="input">
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="class">e_quota</xsl:attribute>
          <xsl:attribute name="value">
            <xsl:value-of select="@quota" />
          </xsl:attribute>
        </xsl:element>


        <input class="editemail" type="submit" value="" title="Ulozit" />
    </li>
  </xsl:template>

  <xsl:template match="*"/>

</xsl:stylesheet>
