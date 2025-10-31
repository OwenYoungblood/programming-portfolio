import ParallaxScrollView from '@/components/parallax-scroll-view';
import { ThemedText } from '@/components/themed-text';
import { ThemedView } from '@/components/themed-view';
import { useColorScheme } from '@/hooks/use-color-scheme';
import { ResizeMode, Video } from 'expo-av';
import { Image } from 'expo-image';
import { ColorSchemeName, Dimensions, Platform, StyleSheet, View } from 'react-native';


import { useState } from 'react';
import { TouchableOpacity } from 'react-native'; // or expo-image

type ExampleVideoProps = {
  title: string;
  url: any;
  colorScheme: ColorSchemeName;
  thumbnail: any;
};

function ExampleVideo({ title, url, colorScheme, thumbnail }: ExampleVideoProps) {
  const [showVideo, setShowVideo] = useState(false);

  return (
    <View style={[
      styles.videoBlock,
      { backgroundColor: colorScheme === 'dark' ? '#2a2a2a' : '#e2f6fe' }
    ]}>
      <ThemedText type="defaultSemiBold" style={{ marginBottom: 6 }}>
        {title}
      </ThemedText>
      {showVideo ? (
        <Video
          source={url}
          style={[
            styles.video,
            { backgroundColor: colorScheme === 'dark' ? '#1a1a1a' : '#1f1f1fff' }
          ]}
          useNativeControls
          resizeMode={ResizeMode.CONTAIN}
          isLooping
          shouldPlay
        />
      ) : (
        <TouchableOpacity onPress={() => setShowVideo(true)}>
          <Image
            source={thumbnail}
            style={styles.video}
            resizeMode="cover"
          />
          {/* You can overlay a play icon here if you wish */}
        </TouchableOpacity>
      )}
    </View>
  );
}


export default function LearningTab() {
  const colorScheme = useColorScheme();
  
  return (
    <ParallaxScrollView
      headerBackgroundColor={{ light: '#e4edf5', dark: '#ffffffff' }}
      headerImage={
        <Image
          source={require('@/assets/images/requestImage.png')}
          style={styles.roofMRILogo}
        />
      }
    >
      <ThemedView style={styles.titleContainer}>
        <ThemedText type="title">How To Videos</ThemedText>
      </ThemedView>

      <ThemedText style={{ marginBottom: 18, marginTop: 4, textAlign: 'center'}}>
        Watch the videos below for a quick refresher on setting up!
      </ThemedText>

      <ThemedView style={styles.videoSection}>
        <ThemedText type="subtitle" style={styles.videoTitle}>
          Setting Up Equipment
        </ThemedText>
        <ExampleVideo
          title="Step 1: Setting Up Equipment"
          url={require('@/assets/videos/settingUpEquipment.mp4')}
          colorScheme={colorScheme}
          thumbnail={require('@/assets/images/settingUpEquipment.png')}
        />
        <ThemedText style={styles.textBelowVideo}>
          Learn how to set up the devices.
        </ThemedText>
      </ThemedView>

      <ThemedView style={styles.videoSection}>
        <ThemedText type="subtitle" style={styles.videoTitle}>
          Calibration
        </ThemedText>
        <ExampleVideo
          title="Step 2: Calibrating"
          url={require('@/assets/videos/calibrating.mp4')}
          colorScheme={colorScheme}
          thumbnail={require('@/assets/images/calibration.png')}
        />
        <ThemedText style={styles.textBelowVideo}>
          Learn how to calibrate the devices.
        </ThemedText>
      </ThemedView>

      <ThemedView style={styles.videoSection}>
        <ThemedText type="subtitle" style={styles.videoTitle}>
          Scanning
        </ThemedText>
        <ExampleVideo
          title="Step 3: Scanning"
          url={require('@/assets/videos/scanning.mp4')}
          colorScheme={colorScheme}
          thumbnail={require('@/assets/images/scanning.png')}
        />
        <ThemedText style={styles.textBelowVideo}>
          Learn how to Scan with the devices.
        </ThemedText>
      </ThemedView>
    </ParallaxScrollView>
  );
}

const windowWidth = Dimensions.get('window').width;
export const styles = StyleSheet.create({
  titleContainer: {
    flexDirection: 'row',
    alignItems: 'center',
    gap: 6,
    paddingHorizontal: 6,
    marginBottom: 12,
    alignSelf: 'center'
  },
  videoSection: {
    marginBottom: 40,
    backgroundColor: '#e2f6fe',
    alignSelf: 'center',
    borderRadius: 24,
    // Conditional width: 100% on mobile, 40% on web
    width: Platform.OS === 'web' ? windowWidth * 0.4 : '100%',
  },
  videoTitle: {
    fontSize: 24,
    fontWeight: 'bold',
    marginBottom: 16,
    textAlign: 'center',
  },
  videoBlock: {
    height: 250,
    width: 320,           // slightly bigger than video width (300)
    padding: 6,
    marginVertical: 10,
    justifyContent: 'center',
    alignItems: 'center',
    borderRadius: 24,
    alignSelf: 'center',  // center horizontally
  },
  video: {
      width: 300,
      height: 200,
      borderRadius: 10,
  },

  textBelowVideo: {
    marginTop: 16,
    marginBottom: 10,
    alignSelf: 'center',
    textAlign: 'center',
    width: '90%',
    fontSize: 23,
    maxWidth: 500,
  },
  logoContainer: {
    alignItems: "center",
    marginBottom: 16,
  },
  roofMRILogo: {
    height: 270 * 0.83,
    width: 392 * 0.88,
    alignSelf: 'center',
    marginVertical: 20,
    // If you want absolute positioning, use this carefully:
    // position: 'absolute',
    // top: 0,
    // left: '50%',
    // marginLeft: -196, // half of width to center absolutely
  },
});
