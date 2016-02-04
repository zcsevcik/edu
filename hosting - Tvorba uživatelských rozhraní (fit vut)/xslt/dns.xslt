<?xml version="1.0" encoding="utf-8"?>
<!--
 * dns.xslt:       Sprava hostingu (ITU 2012)
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
    <form><div class="dns">
      <xsl:apply-templates select="//domain[@login = $login]"/>
    </div></form>
  </xsl:template>

  <xsl:template match="//domain">
    <input type="submit" value="" title="Přidat doménu" id="add_dom" class="add" />
    <div class="domena">domena:</div>

    <xsl:element name="input">
        <xsl:attribute name="type">text</xsl:attribute>
        <xsl:attribute name="class">dom</xsl:attribute>
        <xsl:attribute name="value">
            <xsl:value-of select="@name" />
        </xsl:attribute>
        <xsl:attribute name="name">
            <xsl:value-of select="@id" />
        </xsl:attribute>
        <xsl:attribute name="onchange">
            change_domain(this);
        </xsl:attribute>
    </xsl:element>

    <!--input class="edit" type="submit" value="" title="Ulozit" /-->
    <input type="submit" value="" title="Smazat doménu" id="del_dom" class="del" />
    <div class="cl"></div>
    <ul>
        <xsl:apply-templates select="record"/>
    </ul>
    <div class="cl"></div>
    <input type="submit" value="" title="Přidat IP" id="add_ip" class="add" />
    <div class="cl"></div>
  </xsl:template>

  <xsl:template match="record">
    <li>
        <div class="ip_tname">název</div>
        <xsl:element name="input">
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="name">
            <xsl:value-of select="@id" />
          </xsl:attribute>
          <xsl:attribute name="class">name</xsl:attribute>
          <xsl:attribute name="value">
            <xsl:value-of select="@name" />
          </xsl:attribute>
          <xsl:attribute name="onchange">
            change_domain_name(this);
          </xsl:attribute>
        </xsl:element>

        <div class="ip_t4">ip4:</div>
        <xsl:element name="input">
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="name">
            <xsl:value-of select="@id" />
          </xsl:attribute>
          <xsl:attribute name="class">ip4</xsl:attribute>
          <xsl:attribute name="value">
            <xsl:value-of select="@a" />
          </xsl:attribute>
          <xsl:attribute name="onchange">
            change_domain_4(this);
          </xsl:attribute>
        </xsl:element>

        <!--input class="editip" type="submit" value="" title="Ulozit" /-->
        <div class="ip_t6">ip6:</div>
        <xsl:element name="input">
          <xsl:attribute name="type">text</xsl:attribute>
          <xsl:attribute name="name">
            <xsl:value-of select="@id" />
          </xsl:attribute>
          <xsl:attribute name="class">ip6</xsl:attribute>
          <xsl:attribute name="value">
            <xsl:value-of select="@aaaa" />
          </xsl:attribute>
          <xsl:attribute name="onchange">
            change_domain_6(this);
          </xsl:attribute>
        </xsl:element>

        <!--input class="editip" type="submit" value="" title="Ulozit" /-->
        <input type="submit" value="" title="Smazat IP" id="del_ip" class="del" />
    </li>
  </xsl:template>

  <xsl:template match="*"/>

</xsl:stylesheet>
