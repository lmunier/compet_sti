import cv2


def main():
    cam = cv2.VideoCapture(0)

    while True:
        ret, frame = cam.read()

        if not ret:
            break

        cv2.imshow("Preview", frame)

        if cv2.waitKey(1) & 0xFF is ord('t'):
            cv2.imwrite(filename='pictBottles_test.jpg', img=frame)

            cam.release()
            break


if __name__ == '__main__':
    main()

    cv2.destroyAllWindows()