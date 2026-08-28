namespace Engine
{
    public struct Color
    {
        public float R, G, B, A;

        public Color(float scalar)
        {
            R = scalar;
            G = scalar;
            B = scalar;
            A = scalar;
        }

        public Color(float r, float g, float b, float a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }
    }
}
