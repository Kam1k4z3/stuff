# sucks

import os
import re
import time
from pypresence import Presence
import requests
import spotipy
from spotipy.oauth2 import SpotifyOAuth
SPOTIFY_CLIENT_ID = 'CLIENT_ID_SPOTIFY'
SPOTIFY_CLIENT_SECRET = 'CLIENT_SECRET_SPOTIFY'
SPOTIFY_REDIRECT_URI = 'http://localhost:8888/callback'

SCOPE = 'user-read-currently-playing'

LYRICS_API_URL = "https://lrclib.net/api/get"
cid = "DISCORD_CLIENT_ID"
RPC = Presence(cid)
RPC.connect()


def setup_spotify_client():
    return spotipy.Spotify(auth_manager=SpotifyOAuth(
        client_id=SPOTIFY_CLIENT_ID,
        client_secret=SPOTIFY_CLIENT_SECRET,
        redirect_uri=SPOTIFY_REDIRECT_URI,
        scope=SCOPE
    ))

def get_current_track(sp_client):
    try:
        current_track = sp_client.current_user_playing_track()
        if current_track is None:
            print("No track currently playing or unable to fetch data.")
            return None
        
        track_name = current_track['item']['name']
        artists = [artist['name'] for artist in current_track['item']['artists']]
        artist_names = ', '.join(artists)
        progress_ms = current_track['progress_ms']
        duration_ms = current_track['item']['duration_ms']
        
        return {
            'track': track_name,
            'artists': artist_names,
            'progress_ms': progress_ms,
            'duration_ms': duration_ms
        }
    except Exception as e:
        print(f"Error getting current track: {e}")
        return None

def get_lrc_lyrics(track_name, artist_name):
    try:
        params = {
            "track_name": track_name,
            "artist_name": artist_name,
            "duration": None
        }
        
        response = requests.get(LYRICS_API_URL, params=params)
        response.raise_for_status()
        
        lyrics_data = response.json()
        if not lyrics_data.get('syncedLyrics'):
            print("No synced lyrics (LRC) available for this track.")
            return None
            
        return lyrics_data['syncedLyrics']
    except requests.exceptions.RequestException as e:
        print(f"Error fetching lyrics: {e}")
        return None

def parse_lrc(lrc_text):
    lines = lrc_text.split('\n')
    lyrics = []
    
    timestamp_re = re.compile(r'\[(\d+):(\d+)\.(\d+)\]')
    
    for line in lines:
        timestamps = timestamp_re.findall(line)
        if not timestamps:
            continue
            
        text = timestamp_re.split(line)[-1].strip()
        if not text:
            continue
            
        for min, sec, centisec in timestamps:
            min = int(min)
            sec = int(sec)
            centisec = int(centisec)
            ms = (min * 60 + sec) * 1000 + centisec * 10
            lyrics.append({'time': ms, 'text': text})
    
    lyrics.sort(key=lambda x: x['time'])
    return lyrics

def get_current_verse(lyrics, current_time_ms):
    if not lyrics:
        return "No lyrics available"
    
    current_verse = None
    for i in range(len(lyrics)):
        if lyrics[i]['time'] > current_time_ms:
            break
        current_verse = lyrics[i]['text']
    
    return current_verse if current_verse else "Beginning of song"

def main():
    sp_client = setup_spotify_client()
    
    while True:
        track_info = get_current_track(sp_client)
        if not track_info:
            time.sleep(5)
            continue
        
        print(f"\nCurrently playing: {track_info['track']} by {track_info['artists']}")
        print(f"Progress: {track_info['progress_ms']/1000:.1f}s / {track_info['duration_ms']/1000:.1f}s")
        lrc_text = get_lrc_lyrics(track_info['track'], track_info['artists'])
        if not lrc_text:
            time.sleep(5)
            continue
            
        lyrics = parse_lrc(lrc_text)
        if not lyrics:
            print("No valid lyrics found in LRC format.")
            time.sleep(5)
            continue
            
        current_verse = get_current_verse(lyrics, track_info['progress_ms'])
        print(f"Current verse: {current_verse}")
        RPC.update(
        details=f"\nCurrently playing: {track_info['track']} by {track_info['artists']}",
        state=current_verse)
        
        time.sleep(5)

if __name__ == "__main__":
    main()
