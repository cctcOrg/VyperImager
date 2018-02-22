# App Flow
This document explains how the application flow works

## Components
- AppWin
- AppBook
- ControlButton
- Page

## Pages
Within each individual page, the field `next_page` should define which page is
the next page. The reasoning for this is that allowing the page to specify what
is next allows the page itself to have elements that affect the behavior of the
"Next" button, as the button belongs to the AppBook
