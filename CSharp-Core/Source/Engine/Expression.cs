
namespace Engine 
{
    public class Expression 
    {
        public Expression(string expression) 
        {
            m_ExpressionString = expression;
        }

        public float Evaluate(float x) 
        {
            //return InternalCalls.Expression_Evaluate(m_ExpressionString, x);
            return 0.0f;
        }

        private string m_ExpressionString;
    }
}