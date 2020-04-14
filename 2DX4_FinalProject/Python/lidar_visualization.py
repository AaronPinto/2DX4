import threading
import math
import numpy as np
import open3d as o3d
import queue
import serial


# https://github.com/pyserial/pyserial/issues/216#issuecomment-369414522
class ReadLine:
    def __init__(self, s):
        self.buf = bytearray()
        self.s = s

    def readline(self):
        i = self.buf.find(b"\n")

        if i >= 0:
            r = self.buf[:i + 1]
            self.buf = self.buf[i + 1:]
            return r

        while True:
            i = max(1, min(2048, self.s.in_waiting))
            data = self.s.read(i)
            i = data.find(b"\n")

            if i >= 0:
                r = self.buf + data[:i + 1]
                self.buf[0:] = data[i + 1:]
                return r
            else:
                self.buf.extend(data)


def read_serial_input(in_q):
    s = serial.Serial("COM3", 115200)
    r = ReadLine(s)
    # s.write(b'1')  # This sends the start cmd ('1' or 0x31)

    while True:
        line = r.readline().decode().split(", ")  # read one line
        print(line)
        in_q.put(line)


# Handle the serial I/O in a separate thread, so that we can update the visualization on-the-fly
input_queue = queue.Queue()
input_thread = threading.Thread(target=read_serial_input, args=(input_queue,), daemon=True)
input_thread.start()

points = []
num_slices = 2
dist_per_slice = 200.0  # mm

while True:
    if input_queue.qsize() > 0:
        val = input_queue.get()

        if len(val) == 3:
            x, dist, angle = [float(i) for i in val]
            rad = math.radians(angle)
            z = dist * math.cos(rad)
            y = dist * math.sin(rad)
            points.append([x, y, z])

            if x >= (num_slices - 1) * dist_per_slice and angle >= 359.2:
                break

# Only overwrite the file if we've successfully acquired a full data set
file_name = "demo.xyz"

with open(file_name, 'w') as f:
    f.writelines('{0} {1} {2}\n'.format(*xyz) for xyz in points)
print("Done")

points_per_slice = 512
pcd = o3d.io.read_point_cloud(file_name, format='xyz')

# http://www.open3d.org/docs/release/tutorial/Advanced/non_blocking_visualization.html
vis = o3d.visualization.Visualizer()
vis.create_window()
vis.add_geometry(o3d.geometry.TriangleMesh().create_coordinate_frame(size=100.0))

lines = []
line_set = o3d.geometry.LineSet(points=o3d.utility.Vector3dVector(np.asarray(pcd.points)),
                                lines=o3d.utility.Vector2iVector(lines))
for x in range(num_slices):
    # Create a line between consecutive points in the slice, last point connects to first one
    for i in range(points_per_slice):
        if i == points_per_slice - 1:
            lines.append([i + x * points_per_slice, 0 + x * points_per_slice])
        else:
            lines.append([i + x * points_per_slice, i + x * points_per_slice + 1])

    # Update shown geometry
    vis.remove_geometry(line_set)
    line_set = o3d.geometry.LineSet(points=o3d.utility.Vector3dVector(np.asarray(pcd.points)),
                                    lines=o3d.utility.Vector2iVector(lines))
    vis.add_geometry(line_set)
    vis.poll_events()
    vis.update_renderer()

    # Create lines between corresponding points, joining the slices together
    if x != num_slices - 1:
        for i in range(0, points_per_slice, 4):
            lines.append([i + x * points_per_slice, i + (x + 1) * points_per_slice])

# line_set.rotate([[0, 0, 1], [0, 1, 0], [-1, 0, 0]], center=False)
vis.run()
