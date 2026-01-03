#!/usr/bin/env bash
deno compile --allow-net --allow-read --allow-env --allow-write ask2ai.ts
cp ask2ai ../server/
rm -f ask2ai

