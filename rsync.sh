#!/bin/sh

MAC020="/Users/jeroenjonkman/Developer/Sensors/"
MAC001="/Volumes/jeroenjonkman/Developer/Sensors/"
rsync -taruvg ${MAC020} ${MAC001}
rsync -taruvg ${MAC001} ${MAC020}
